#include "record.h"

#include "constants.h"
#include "data.h"
#include "io.h"
#include "logger.h"
#include "rest.h"
#include "sort.h"

#include <stdio.h>
#include <string.h>

#define READ_TABLE_FAILURE_FORMAT "Failed to read a RecordTable from %s"

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
Record make_record(const char *name, const int score) {
  Record record;

  /* Safely copy the provided name into the array. */
  strncpy(record.name, name, MAXIMUM_PLAYER_NAME_SIZE - 1);
  /* We must manually assure that the last char is a null character. */
  record.name[MAXIMUM_PLAYER_NAME_SIZE - 1] = '\0';

  record.score = score;

  return record;
}

int compare_records(const Record *const a, const Record *const b) {
  if (a->score < b->score) {
    return -1;
  } else if (a->score == b->score) {
    return 0;
  } else {
    return 1;
  };
}

int compare_void_record_pointers(const void *a, const void *b) {
  return compare_records((const Record *)a, (const Record *)b);
}

/**
 * Writes the default records to the RecordTable.
 *
 * All records are properly initialized, independently of the table size.
 */
void populate_table_with_default_records(RecordTable *table) {
  Record empty_record;
  static char *names[] = {"Adam", "Bree", "Cora", "Dave", "Elmo"};
  static int scores[] = {18, 14, 10, 8, 2};
  size_t i;
  size_t end = sizeof(scores) / sizeof(int);
  if (RECORD_ARRAY_SIZE < end) {
    end = RECORD_ARRAY_SIZE;
  }
  table->record_count = 0;
  for (i = 0; i < end; i++) {
    table->records[i] = make_record(names[i], scores[i]);
    table->record_count++;
  }
  /* Safely fill the rest of the table with the empty record. */
  memset(empty_record.name, '\0', MAXIMUM_PLAYER_NAME_SIZE);
  empty_record.score = 0;
  while (i < RECORD_ARRAY_SIZE) {
    table->records[i] = empty_record;
    i++;
  }
}

/**
 * Reads a RecordTable into the provided destination.
 *
 * If the existing table cannot be used a new one is created.
 */
void read_table(RecordTable *table) {
  int read_error = 0;
  char log_buffer[MAXIMUM_STRING_SIZE];
  if (file_exists(RECORD_TABLE_FILENAME)) {
    if (read_bytes(RECORD_TABLE_FILENAME, table, sizeof(RecordTable), 1)) {
      sprintf(log_buffer, READ_TABLE_FAILURE_FORMAT, RECORD_TABLE_FILENAME);
      log_message(log_buffer);
      /* Set the error flag to trigger the creation of a new table. */
      read_error = 1;
    }
  } else {
    /* Set the error flag to trigger the creation of a new table. */
    read_error = 1;
  }
  if (read_error) {
    populate_table_with_default_records(table);
    log_message("Populated the table with the default records");
  }
}

void write_table(const RecordTable *const table) {
  write_bytes(RECORD_TABLE_FILENAME, table, sizeof(RecordTable), 1);
}

/**
 * Writes the specified Record to to the system.
 */
int save_record(const Record *const record) {
  int record_index;
  RecordTable table;
  read_table(&table);

  /* If the table is full, overwrite the last record if this record is greater.
   */
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

  /* Sort the table records. */
  qsort((void *)table.records, table.record_count, sizeof(Record),
        compare_void_record_pointers);
  reverse((void *)table.records, table.record_count, sizeof(Record));
  log_message("Sorted the record table");

  /* Write the table to disk. */
  write_table(&table);
  /* Find the index of the provided record. */
  record_index = table.record_count - 1;
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
size_t read_records(Record *destination, size_t destination_size) {
  RecordTable table;
  size_t i;
  size_t max_i;
  read_table(&table);
  max_i = table.record_count;
  if (destination_size < table.record_count) {
    /* Do not copy more than the caller asked for. */
    max_i = destination_size;
  }
  for (i = 0; i < max_i; i++) {
    destination[i] = table.records[i];
  }
  return i;
}

/**
 * Returns the number of characters used to represent the provided number on
 * base 10.
 */
int count_digits(int number) {
  char buffer[MAXIMUM_STRING_SIZE];
  sprintf(buffer, "%d", number);
  return strlen(buffer);
}

void record_to_string(const Record *const record, char *buffer,
                      const int expected_width) {
  const char format[] = "%s%*.*s%d";
  char padding_string[MAXIMUM_STRING_SIZE];
  int padding_length;
  memset(padding_string, '.', MAXIMUM_STRING_SIZE - 1);
  padding_string[MAXIMUM_STRING_SIZE - 1] = '\0';
  padding_length =
      expected_width - strlen(record->name) - count_digits(record->score);
  sprintf(buffer, format, record->name, padding_length, padding_length,
          padding_string, record->score);
}

/**
 * Loads and presents the top scores on the screen.
 */
void top_scores(SDL_Renderer *renderer) {
  Record records[MAXIMUM_DISPLAYED_RECORDS];
  char line[MAXIMUM_STRING_SIZE];
  const int padding = 2;
  const int line_width = COLUMNS - 6;
  const int line_count = LINES - 2 * padding;
  const int record_width = min(line_width, MAXIMUM_STRING_SIZE - 1);
  const size_t record_count = read_records(records, MAXIMUM_DISPLAYED_RECORDS);
  size_t i;
  if (COLUMNS < 16) {
    return;
  }
  clean(renderer);
  for (i = 0; i < record_count && i < line_count; i++) {
    record_to_string(records + i, line, record_width);
    print_centered(padding + i, line, renderer);
  }
  present(renderer);
  rest_for_seconds(3);
}
