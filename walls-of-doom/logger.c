#include "logger.h"
#include "clock.h"
#include "constants.h"
#include "data.h"
#include "score.h"
#include "settings.h"
#include "version.h"
#include <stdio.h>
#include <time.h>

#define LOGGER_VERSION_MESSAGE "Version is " WALLS_OF_DOOM_VERSION "."

#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define TIMESTAMP_BUFFER_SIZE 64

#define LOG_FILE_NAME "log.txt"

#define SCORE_FILE_NAME "score.txt"

#define LOG_MESSAGE_SIZE (TIMESTAMP_BUFFER_SIZE + 256)

/**
 * Initializes the logger. Should only be called once.
 */
void initialize_logger(void) {
  /* Currently, the logger does not need initialization. */
  log_message("Initialized the logger.");
  log_message(LOGGER_VERSION_MESSAGE);
}

/**
 * Terminates the logger. Should only be called once.
 */
void finalize_logger(void) {
  /* Currently, the logger does not need termination. */
  log_message("Finalized the logger.");
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

static void append_to_file(const char *path, const char *string) {
  FILE *file;
  file = fopen(path, "a");
  if (file) {
    fprintf(file, "%s\n", string);
    fclose(file);
  }
}

/**
 * Logs the provided message to the current log file.
 */
void log_message(const char *message) {
  /*
   * Note that this function CANNOT use resize_memory because resize_memory
   * calls this function. Using resize_memory here may cause a deadlock.
   */
  char path[MAXIMUM_PATH_SIZE];
  char stamp[TIMESTAMP_BUFFER_SIZE];
  char string[LOG_MESSAGE_SIZE];
  /* get_full_path does not use dynamic memory allocation. */
  get_full_path(path, LOG_FILE_NAME);
  /* write_timestamp does not use dynamic memory allocation. */
  write_timestamp(stamp, TIMESTAMP_BUFFER_SIZE);
  sprintf(string, "[%s] %s", stamp, message);
  /* append_to_file does not use dynamic memory allocation. */
  append_to_file(path, string);
}

void log_player_score(const unsigned long frame, const Score score) {
  char path[MAXIMUM_PATH_SIZE];
  char string[LOG_MESSAGE_SIZE];
  if (is_logging_player_score()) {
    get_full_path(path, SCORE_FILE_NAME);
    sprintf(string, "%ld,%ld", frame, score);
    append_to_file(path, string);
  }
}
