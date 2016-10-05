#include "data.h"

#include "code.h"
#include "constants.h"
#include "logger.h"

#include <pwd.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define LOG_ACCESS_WRITE_FORMAT "Writing %lu bytes (%.2lf KiB) to %s"
#define LOG_ACCESS_READ_FORMAT "Reading %lu bytes (%.2lf KiB) from %s"

#define WRITE_BYTES_COUNT_FORMAT "Expected to write %lu but actually wrote %lu"
#define READ_BYTES_COUNT_FORMAT "Expected to read %lu but actually read %lu"

#define DATA_DIRECTORY ".walls-of-doom"

/* Data directory is writable for the user, and read-only for others. */
#define DATA_DIRECTORY_UMASK 0755

/**
 * Assesses whether or not a file with the provided filename exists.
 */
int file_exists(const char *filename) {
  struct stat buffer;
  return stat(filename, &buffer) == 0;
}

typedef enum Operation { READ, WRITE } Operation;

/**
 * Writes to buffer the full path for a file created by Walls of Doom.
 *
 * If one needs to access the log.txt file, one should use
 *
 *   char path[MAXIMUM_PATH_SIZE];
 *   get_full_path(path, "log.txt");
 *
 * This is the correct way to access mutable files in any platform.
 *
 * This function does not rely on dynamic memory allocation.
 */
Code get_full_path(char *buffer, char *filename) {
  const char *home;
  struct stat status;
  size_t path_size = 0;
  /* Get the home directory. */
  if ((home = getenv("HOME")) == NULL) {
    /* Only use the user database if HOME is not available. */
    home = getpwuid(getuid())->pw_dir;
  }
  /* Check if the full path fits in the buffer. */
  path_size = strlen(home);
  path_size += SEPARATOR_SIZE;
  path_size += strlen(DATA_DIRECTORY);
  path_size += SEPARATOR_SIZE;
  path_size += strlen(filename);
  path_size += 1;
  if (path_size > MAXIMUM_PATH_SIZE) {
    return CODE_ERROR;
  }
  /* Create the data directory if it does not exist. */
  sprintf(buffer, "%s/%s", home, DATA_DIRECTORY);
  if (stat(buffer, &status) == -1) {
    mkdir(buffer, DATA_DIRECTORY_UMASK);
  }
  /* Should not write from one buffer to the same buffer. */
  sprintf(buffer, "%s/%s/%s", home, DATA_DIRECTORY, filename);
  return CODE_OK;
}

/**
 * Returns the number of lines in a file.
 *
 * This is the count of occurrences of '\n'.
 */
int file_line_count(const char *filename) {
  int line_count = 0;
  int read;
  FILE *file = fopen(filename, "r");
  if (file) {
    while ((read = fgetc(file)) != EOF) {
      if (read == '\n') {
        line_count++;
      }
    }
    fclose(file);
  }
  return line_count;
}

void log_access(Operation operation, const size_t bytes, const char *filename) {
  char message[MAXIMUM_STRING_SIZE];
  double kibibytes = bytes / 1024;
  char *format = NULL;
  if (operation == READ) {
    format = LOG_ACCESS_READ_FORMAT;
  } else {
    format = LOG_ACCESS_WRITE_FORMAT;
  }
  sprintf(message, format, bytes, kibibytes, filename);
  log_message(message);
}

/**
 * Writes bytes to the indicated file from the provided source.
 *
 * Returns 0 in case of success.
 */
int write_bytes(const char *filename, const void *source, const size_t size,
                const size_t count) {
  FILE *file;
  size_t written_items;
  char log_buffer[MAXIMUM_STRING_SIZE];
  log_access(WRITE, size * count, filename);
  file = fopen(filename, "wb");
  if (file == NULL) {
    return 1;
  }
  written_items = fwrite(source, size, count, file);
  fclose(file);
  if (written_items != count) {
    sprintf(log_buffer, WRITE_BYTES_COUNT_FORMAT, count, written_items);
    log_message(log_buffer);
    return 2;
  }
  return 0;
}

/**
 * Reads bytes from the indicated file to the provided destination.
 *
 * Returns 0 in case of success.
 */
int read_bytes(const char *filename, void *destination, const size_t size,
               const size_t count) {
  FILE *file;
  size_t read_items;
  char log_buffer[MAXIMUM_STRING_SIZE];
  log_access(READ, size * count, filename);
  if (!file_exists(filename)) {
    return 1;
  }
  file = fopen(filename, "rb");
  if (file == NULL) {
    return 2;
  }
  read_items = fread(destination, size, count, file);
  fclose(file);
  if (read_items != count) {
    sprintf(log_buffer, READ_BYTES_COUNT_FORMAT, count, read_items);
    log_message(log_buffer);
    return 3;
  }
  return 0;
}

/**
 * Reads characters from the indicated file into the provided character string.
 *
 * Returns 0 in case of success.
 */
int read_characters(const char *const filename, char *destination,
                    const size_t destination_size) {
  FILE *file;
  size_t copied = 0;
  int c; /* Must be an integer because it may be EOF */
  log_access(READ, destination_size, filename);
  if (file_exists(filename)) {
    file = fopen(filename, "r");
    if (file) {
      /* Check copied + 1 against destination size because we need a null
       * character at the end. */
      while (copied + 1 < destination_size && (c = fgetc(file)) != EOF) {
        destination[copied] = (char)c;
        copied++;
      }
      /* Done copying, place a null character if we can. */
      if (destination_size > 0) { /* Provided size may be 0. */
        destination[copied] = '\0';
      }
      fclose(file);
      return 0;
    }
  }
  return 1;
}

/**
 * Reads integers from the indicated file into the provided array.
 *
 * Returns the number of integers read.
 */
size_t read_integers(const char *const filename, int *integer_array,
                     const size_t integer_array_size) {
  FILE *file;
  size_t next_index = 0;
  file = fopen(filename, "r");
  if (file) {
    while (next_index < integer_array_size &&
           fscanf(file, "%d", &integer_array[next_index]) != EOF) {
      next_index++;
    }
    fclose(file);
  }
  return next_index; /* number of elements == index of next element */
}
