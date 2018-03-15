#include "record.hpp"
#include "code.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "high-io.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "numeric.hpp"
#include "profiler.hpp"
#include "settings.hpp"
#include "sort.hpp"
#include "text.hpp"
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#define READ_TABLE_BUFFER_SIZE 256
#define RECORD_CSV_OUT "\"%s\",%ld\n"

typedef struct RecordTable {
  size_t record_count;
  Record records[RECORD_ARRAY_SIZE];
} RecordTable;

Record make_record(const char *name, const Score score) {
  Record record;
  /* Safely copy the provided name into the array. */
  memset(record.name, '\0', MAXIMUM_PLAYER_NAME_SIZE);
  copy_string(record.name, name, MAXIMUM_PLAYER_NAME_SIZE);
  record.score = score;
  return record;
}

int compare_records(const Record *const a, const Record *const b) {
  if (a->score < b->score) {
    return -1;
  }
  if (a->score == b->score) {
    return 0;
  } else {
    return 1;
  };
}

int compare_void_records(const void *a, const void *b) { return compare_records((const Record *)a, (const Record *)b); }

void populate_table_with_default_records(RecordTable *table) {
  Record empty_record;
  std::vector<std::string> names = {"Adam", "Bree", "Cora", "Dave", "Elmo"};
  static int scores[] = {18, 14, 10, 8, 2};
  size_t i;
  size_t end = sizeof(scores) / sizeof(int);
  if (RECORD_ARRAY_SIZE < end) {
    end = RECORD_ARRAY_SIZE;
  }
  table->record_count = 0;
  for (i = 0; i < end; i++) {
    table->records[i] = make_record(names[i].c_str(), scores[i]);
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

static void parse_record(const char *buffer, Record *const record) {
  const char *begin = NULL;
  const char *end = NULL;
  /* Properly initialize the Record even if parsing fails. */
  record->score = 0;
  record->name[0] = '\0';
  if (*buffer == '"') {
    begin = buffer + 1;
    end = strchr(buffer + 1, '"');
    if (end == NULL) {
      return;
    }
    copy_string_up_to(record->name, begin, end, MAXIMUM_PLAYER_NAME_SIZE);
    begin = strchr(end, ',');
    if (begin == NULL) {
      return;
    }
    record->score = strtol(begin + 1, NULL, 10);
  }
}

/**
 * Reads a RecordTable into the provided destination.
 *
 * If the existing table cannot be used a new one is created.
 */
void read_table(RecordTable *table) {
  char path[MAXIMUM_PATH_SIZE];
  int read_error = 0;
  Record *record;
  FILE *file;
  size_t i;
  char *buffer = NULL;
  buffer = reinterpret_cast<char *>(resize_memory(buffer, READ_TABLE_BUFFER_SIZE));
  get_full_path(path, RECORD_TABLE_FILE_NAME);
  if (file_exists(path)) {
    file = fopen(path, "r");
    if (file != NULL) {
      table->record_count = 0;
      for (i = 0; i < RECORD_ARRAY_SIZE; i++) {
        record = table->records + i;
        if (fgets(buffer, READ_TABLE_BUFFER_SIZE, file) != NULL) {
          table->record_count++;
          parse_record(buffer, record);
        } else {
          break;
        }
      }
      fclose(file);
    } else {
      read_error = 1;
    }
  } else {
    /* Set the error flag to trigger the creation of a new table. */
    read_error = 1;
  }
  if (read_error) {
    populate_table_with_default_records(table);
    log_message("Populated the table with the default records.");
  }
  resize_memory(buffer, 0);
}

void write_table(const RecordTable *const table) {
  char path[MAXIMUM_PATH_SIZE];
  const Record *record;
  FILE *file;
  size_t i;
  get_full_path(path, RECORD_TABLE_FILE_NAME);
  file = fopen(path, "w");
  if (file != NULL) {
    for (i = 0; i < table->record_count; i++) {
      record = table->records + i;
      fprintf(file, RECORD_CSV_OUT, record->name, record->score);
    }
    fclose(file);
  }
}

/**
 * Writes the specified Record to to the system.
 */
int save_record(Record *record) {
  int (*comparator)(const void *a, const void *b) = &compare_void_records;
  int record_index;
  RecordTable table;
  read_table(&table);
  /* If the table is full, overwrite the last record if this is greater. */
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
  log_message("Added the record to the record table.");
  /* Sort the table records. */
  sort((void *)table.records, table.record_count, sizeof(Record), comparator);
  reverse((void *)table.records, table.record_count, sizeof(Record));
  log_message("Sorted the record table.");
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
 * Reads Records from the system into the specified destination. Reading at most destination_size Records.
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

Code top_scores(SDL_Renderer *renderer, CommandTable *table) {
  Milliseconds start = get_milliseconds();
  Record records[MAXIMUM_DISPLAYED_RECORDS];
  const size_t count = read_records(records, MAXIMUM_DISPLAYED_RECORDS);
  print_records(count, records, renderer);
  update_profiler("top_scores", get_milliseconds() - start);
  return wait_for_input(table);
}
