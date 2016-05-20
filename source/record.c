#include "record.h"

#include "data.h"
#include "logger.h"
#include "sort.h"

#include <stdio.h>
#include <string.h>

// Get as close to 64 KiB as possible.
#define RECORD_ARRAY_SIZE 963
#define RECORD_TABLE_FILENAME "records.bin"

typedef struct RecordTable {
    size_t record_count;
    Record records[RECORD_ARRAY_SIZE];
} RecordTable;

/**
 * Creates a new Record from the provided data.
 *
 * If the name is too big to fit into the array, it is truncated to the maximum
 * possible size.
 */
Record make_record(const char * name, const int score) {
    Record record;
    
    // Safely copy the provided name into the array.
    strncpy(record.name, name, PLAYER_NAME_MAXIMUM_SIZE - 1);
    // We must manually assure that the last char is a null character.
    record.name[PLAYER_NAME_MAXIMUM_SIZE - 1] = '\0';

    record.score = score;
    
    return record;
}

int compare_records(const Record * const a, const Record * const b) {
    if (a->score < b->score) {
        return -1;
    } else if (a->score == b->score) {
        return 0;
    } else {
        return 1;
    };
}

int compare_void_record_pointers(const void *a, const void *b) {
    return compare_records((const Record *) a, (const Record *) b);
}

/**
 * Reads a RecordTable into the provided destination.
 *
 * If the existing table cannot be used a new one is created.
 */
void read_table(RecordTable * const table) {
    int read_error = 0;
    if (file_exists(RECORD_TABLE_FILENAME)) {
        if (read_bytes(RECORD_TABLE_FILENAME, table, sizeof(RecordTable), 1)) {
            char message[512];
            sprintf(message, "Failed to read a RecordTable from %s", RECORD_TABLE_FILENAME);
            log_message(message);
            read_error = 1; // Set the error flag to trigger the creation of a new table.
        }
    } else {
        read_error = 1; // Set the error flag to trigger the creation of a new table.
    }
    if (read_error) {
        RecordTable empty_table;
        log_message("Created an empty RecordTable");
        // Properly set the record_count field.
        empty_table.record_count = 0;
        *table = empty_table;
    }
}

void write_table(const RecordTable * const table) {
    write_bytes(RECORD_TABLE_FILENAME, table, sizeof(RecordTable), 1);
}

/**
 * Writes the specified Record to to the system.
 */
int save_record(const Record * const record) {
    // Add 1 to the maximum size as we need to store the newest record.
    RecordTable table;
    read_table(&table);

    // If the table is full, overwrite the last record if this record is greater.
    if (table.record_count == RECORD_ARRAY_SIZE) {
        if (compare_records(record, table.records + (table.record_count - 1)) > 0) {
            table.records[table.record_count - 1] = *record;
        } else {
            return -1;
        }
    } else {
        table.records[table.record_count] = *record;
        table.record_count++;
    }
    log_message("Added the record to the record table");

    // Sort the table records.
    insertion_sort((void *)table.records, table.record_count, sizeof(Record), compare_void_record_pointers);
    reverse((void *)table.records, table.record_count, sizeof(Record));
    log_message("Sorted the record table");

    // Write the table to disk.
    write_table(&table);
    // Find the index of the provided record.
    int record_index = table.record_count - 1;
    while (compare_records(record, table.records + record_index) != 0) {
        record_index--;
    }
    return record_index;
}

/**
 * Reads Records from the system into the specified destination. Reading at
 * most destination_size Records.
 *
 * Returns how many Records were actually read.
 */
size_t read_records(Record * destination, size_t destination_size) {
    RecordTable table;
    read_table(&table);
    size_t i;
    const size_t max_i = destination_size <= table.record_count ? destination_size : table.record_count;
    for (i = 0; i < max_i; i++) {
        destination[i] = table.records[i];
    }
    return i;
}
