#include "profiler.h"
#include "clock.h"
#include "constants.h"
#include "data.h"
#include "logger.h"
#include "memory.h"
#include "sort.h"
#include "text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXIMUM_DATA_IDENTIFIER_SIZE 32

#define OUTPUT_HEADER "Mean,Frequency,Identifier\n"
#define OUTPUT_FORMAT "\"%s\",%f,%ld\n"
#define OUTPUT_FORMAT_SIZE 128

typedef struct ProfilerData {
  char identifier[MAXIMUM_DATA_IDENTIFIER_SIZE];
  Milliseconds sum;
  unsigned long frequency;
} ProfilerData;

static size_t table_size = 0;
/**
 * A very unoptimized table.
 *
 * Starts with size 0, increments in steps of one.
 * Linear search is used to find the desired ProfilerData.
 *
 * Performance will degrade if too many different identifiers are used.
 */
static ProfilerData *table = NULL;

Code initialize_profiler(void) { return CODE_OK; }

ProfilerData *get_empty_data(const char *identifier) {
  const size_t new_size = (table_size + 1) * sizeof(ProfilerData);
  ProfilerData *reallocated_table;
  ProfilerData empty_data;
  copy_string(empty_data.identifier, identifier, MAXIMUM_DATA_IDENTIFIER_SIZE);
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

static void profiler_data_copy_base_id(const ProfilerData *data, char *dest) {
  char *colon = strchr(data->identifier, ':');
  copy_string(dest, data->identifier, MAXIMUM_DATA_IDENTIFIER_SIZE);
  if (colon != NULL) {
    /* Cut the string before the colon. */
    dest[colon - data->identifier] = '\0';
  }
}

static int profiler_data_greater_than(const void *a, const void *b) {
  const ProfilerData *a_data = (ProfilerData *)a;
  const ProfilerData *b_data = (ProfilerData *)b;
  const double a_mean = profiler_data_mean(a_data);
  const double b_mean = profiler_data_mean(b_data);
  char a_base_id[MAXIMUM_DATA_IDENTIFIER_SIZE];
  char b_base_id[MAXIMUM_DATA_IDENTIFIER_SIZE];
  int comparison;
  profiler_data_copy_base_id(a_data, a_base_id);
  profiler_data_copy_base_id(b_data, b_base_id);
  comparison = strcmp(a_base_id, b_base_id);
  if (comparison != 0) {
    return comparison;
  }
  if (a_mean < b_mean) {
    return 1;
  }
  if (a_mean == b_mean) {
    return 0;
  }
  return -1;
}

void sort_table(void) {
  sort(table, table_size, sizeof(ProfilerData), profiler_data_greater_than);
}

void write_statistics(void) {
  char path[MAXIMUM_PATH_SIZE];
  unsigned long frequency;
  double mean;
  size_t i;
  char *identifier;
  FILE *file;
  if (table == NULL) {
    return;
  }
  get_full_path(path, PROFILER_FILE_NAME);
  file = fopen(path, "a");
  if (file) {
    /* Sort the table if we can write output. */
    sort_table();
    for (i = 0; i < table_size; i++) {
      mean = profiler_data_mean(table + i);
      frequency = table[i].frequency;
      identifier = table[i].identifier;
      fprintf(file, OUTPUT_FORMAT, identifier, mean, frequency);
    }
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
