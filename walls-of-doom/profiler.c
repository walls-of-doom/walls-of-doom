#include "clock.h"
#include "code.h"
#include "constants.h"
#include "data.h"
#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM_DATA_IDENTIFIER_SIZE 32
#define OUTPUT_FORMAT "%.2f ms  %s\n"

typedef struct ProfilerData {
  char identifier[MAXIMUM_DATA_IDENTIFIER_SIZE];
  Milliseconds sum;
  unsigned long frequency;
} ProfilerData;

size_t table_size = 0;
/**
 * A very unoptimized table.
 *
 * Starts with size 0, increments in steps of one.
 * Linear search is used to find the desired ProfilerData.
 *
 * Performance will degrade if too many different identifiers are used.
 */
ProfilerData *table = NULL;

Code initialize_profiler(void) { return CODE_OK; }

ProfilerData *get_empty_data(const char *identifier) {
  const size_t new_size = (table_size + 1) * sizeof(ProfilerData);
  ProfilerData *reallocated_table;
  ProfilerData empty_data;
  safe_strcpy(&empty_data.identifier, identifier, MAXIMUM_DATA_IDENTIFIER_SIZE);
  empty_data.sum = 0;
  empty_data.frequency = 0;
  reallocated_table = resize_memory(table, new_size);
  if (reallocated_table != NULL) {
    table = reallocated_table;
    table_size++;
    table[table_size - 1] = empty_data;
  } else {
    log_message("Failed to reallocate table");
  }
  return table + table_size - 1;
}

ProfilerData *get_data(const char *identifier) {
  int found = 0;
  size_t i;
  for (i = 0; i < table_size && !found; i++) {
    found = strcmp(identifier, table[i].identifier) == 0;
  }
  if (!found) {
    return get_empty_data(identifier);
  }
  /* Decrement because i is incremented before exiting the loop. */
  return table + i - 1;
}

/**
 * Updates the statistics about an identifier with a new millisecond count.
 */
void update_profiler(const char *identifier, const Milliseconds delta) {
  ProfilerData *data = get_data(identifier);
  data->frequency++;
  data->sum += delta;
}

static double profiler_data_mean(const ProfilerData *const data) {
  return data->sum / (double)data->frequency;
}

static int profiler_data_greater_than(const void *a, const void *b) {
  const double a_mean = profiler_data_mean((ProfilerData *)a);
  const double b_mean = profiler_data_mean((ProfilerData *)b);
  if (a_mean < b_mean) {
    return 1;
  }
  if (a_mean == b_mean) {
    return 0;
  }
  return -1;
}

void sort_table(void) {
  qsort(table, table_size, sizeof(ProfilerData), profiler_data_greater_than);
}

void write_statistics(void) {
  size_t i;
  double average;
  char path[MAXIMUM_PATH_SIZE];
  get_full_path(path, PROFILER_FILE_NAME);
  sort_table();
  FILE *file = fopen(path, "a");
  if (file) {
    for (i = 0; i < table_size; i++) {
      average = profiler_data_mean(table + i);
      fprintf(file, OUTPUT_FORMAT, average, table[i].identifier);
    }
    fprintf(file, "\n");
    fclose(file);
  }
}

/**
 * Saves all profiler data to disk and frees the allocated memory.
 */
Code finalize_profiler(void) {
  write_statistics();
  table = resize_memory(table, 0);
  table_size = 0;
  log_message("Freed the profiler table");
  return CODE_OK;
}
