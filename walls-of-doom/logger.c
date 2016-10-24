#include "logger.h"
#include "clock.h"
#include "constants.h"
#include "data.h"
#include "version.h"
#include <stdio.h>
#include <time.h>

#define LOGGER_VERSION_MESSAGE "Version: " WALLS_OF_DOOM_VERSION

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_BUFFER_SIZE 64

/**
 * Initializes the logger. Should only be called once.
 */
void initialize_logger(void) {
  /* Currently, the logger does not need initialization. */
  log_message("Initialized the logger");
  log_message(LOGGER_VERSION_MESSAGE);
}

/**
 * Terminates the logger. Should only be called once.
 */
void finalize_logger(void) {
  /* Currently, the logger does not need termination. */
  log_message("Finalized the logger");
}

/**
 * Writes a timestamp to the provided buffer.
 *
 * TIMESTAMP_BUFFER_SIZE is the minimum size for the timestamp buffer.
 *
 * This function does not rely on dynamic memory allocation.
 */
static void write_timestamp(char *buffer, const size_t buffer_size) {
  time_t current_time;
  struct tm *time_info;
  time(&current_time);
  time_info = localtime(&current_time);
  strftime(buffer, buffer_size, TIMESTAMP_FORMAT, time_info);
}

/**
 * Logs the provided message to the current log file.
 */
void log_message(const char *message) {
  /*
   * Note that this function CANNOT use resize_memory because resize_memory
   * calls this function. Using resize_memory here may cause a deadlock.
   */
  FILE *file;
  char buffer[TIMESTAMP_BUFFER_SIZE];
  char path[MAXIMUM_PATH_SIZE];
  /* get_full_path does not use dynamic memory allocation. */
  get_full_path(path, LOG_FILE_NAME);
  file = fopen(path, "a");
  if (file) {
    /* write_timestamp does not use dynamic memory allocation. */
    write_timestamp(buffer, TIMESTAMP_BUFFER_SIZE);
    fprintf(file, "[%s] %s\n", buffer, message);
    fclose(file);
  }
}
