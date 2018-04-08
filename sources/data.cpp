#include "data.hpp"
#include "code.hpp"
#include "constants.hpp"
#include "logger.hpp"
#include "text.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define WRITE_BYTES_COUNT_FORMAT "Expected to write %lu but actually wrote %lu!"
#define READ_BYTES_COUNT_FORMAT "Expected to read %lu but actually read %lu!"

#define DATA_DIRECTORY "data"

/* Data directory is writable for the user, and read-only for others. */
#define DATA_DIRECTORY_UMASK 0755

/**
 * Assesses whether or not a file with the provided filename exists.
 */
bool file_exists(const char *filename) {
  struct stat buffer {};
  return stat(filename, &buffer) == 0;
}

typedef enum Operation { READ, WRITE } Operation;

/**
 * Writes to buffer the full path for a file created by Walls of Doom.
 *
 * If one needs to access the log.txt file, one should use
 *
 *   char path[MAXIMUM_PATH_SIZE];
 *   get_full_path(path, filename);
 *
 * This is the correct way to access mutable files in any platform.
 *
 * This function does not rely on dynamic memory allocation.
 */
Code get_full_path(char *buffer, const char *filename) {
  struct stat status {};
  size_t path_size = 0;
  /* Check if the full path fits in the buffer. */
  path_size = strlen(DATA_DIRECTORY);
  path_size += SEPARATOR_SIZE;
  path_size += strlen(filename);
  path_size += 1;
  if (path_size > MAXIMUM_PATH_SIZE) {
    return CODE_ERROR;
  }
  /* Create the data directory if it does not exist. */
  sprintf(buffer, "%s", DATA_DIRECTORY);
  if (stat(buffer, &status) == -1) {
#ifdef _WIN32
    CreateDirectory(buffer, NULL);
#else
    mkdir(buffer, DATA_DIRECTORY_UMASK);
#endif
  }
  /* Should not write from one buffer to the same buffer. */
  sprintf(buffer, "%s/%s", DATA_DIRECTORY, filename);
  return CODE_OK;
}

std::string get_full_path(const std::string &filename) {
  char path[MAXIMUM_PATH_SIZE];
  if (get_full_path(path, filename.c_str()) != CODE_OK) {
    throw std::runtime_error("Failed to get full path.");
  }
  return std::string(path);
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
  if (file != nullptr) {
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
  const auto kibibytes = bytes / 1024.0;
  const auto bytes_string = std::to_string(bytes);
  const auto kibibytes_string = double_to_string(kibibytes, 2);
  std::string message;
  if (operation == READ) {
    message = "Reading " + bytes_string + " bytes (" + kibibytes_string + " KiB) from " + filename + ".";
  } else {
    message = "Writing " + bytes_string + " bytes (" + kibibytes_string + " KiB) to " + filename + ".";
  }
  log_message(message);
}

/**
 * Writes bytes to the indicated file from the provided source.
 */
Code write_bytes(const char *filename, const void *source, const size_t size, const size_t count) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  U64 long_count;
  U64 long_written;
  size_t written;
  FILE *file;
  log_access(WRITE, size * count, filename);
  file = fopen(filename, "wb");
  if (file == nullptr) {
    return CODE_ERROR;
  }
  written = fwrite(source, size, count, file);
  fclose(file);
  if (written != count) {
    long_count = static_cast<U64>(count);
    long_written = static_cast<U64>(written);
    sprintf(log_buffer, WRITE_BYTES_COUNT_FORMAT, long_count, long_written);
    log_message(log_buffer);
    return CODE_ERROR;
  }
  return CODE_OK;
}

Code write_string(const char *filename, const std::string &string) {
  return write_bytes(filename, string.c_str(), 1, string.size());
}

/**
 * Reads bytes from the indicated file to the provided destination.
 */
Code read_bytes(const char *filename, void *destination, const size_t size, const size_t count) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  U64 long_count;
  U64 long_read;
  size_t read;
  FILE *file;
  log_access(READ, size * count, filename);
  if (!file_exists(filename)) {
    return CODE_ERROR;
  }
  file = fopen(filename, "rb");
  if (file == nullptr) {
    return CODE_ERROR;
  }
  read = fread(destination, size, count, file);
  fclose(file);
  if (read != count) {
    long_count = static_cast<U64>(count);
    long_read = static_cast<U64>(read);
    sprintf(log_buffer, READ_BYTES_COUNT_FORMAT, long_count, long_read);
    log_message(log_buffer);
    return CODE_ERROR;
  }
  return CODE_OK;
}

/**
 * Reads characters from the indicated file into the provided character string.
 */
Code read_characters(const char *const filename, char *destination, const size_t destination_size) {
  FILE *file;
  size_t copied = 0;
  int c; /* Must be an integer because it may be EOF */
  log_access(READ, destination_size, filename);
  if (file_exists(filename)) {
    file = fopen(filename, "r");
    if (file != nullptr) {
      /* Check copied + 1 against destination size because we need a null
       * character at the end. */
      while (copied + 1 < destination_size && (c = fgetc(file)) != EOF) {
        destination[copied] = static_cast<char>(c);
        copied++;
      }
      /* Done copying, place a null character if we can. */
      if (destination_size > 0) { /* Provided size may be 0. */
        destination[copied] = '\0';
      }
      fclose(file);
      return CODE_OK;
    }
  }
  return CODE_ERROR;
}

Code read_characters(const char *const filename, std::string &destination) {
  std::ifstream stream(filename);
  if (!stream) {
    return CODE_ERROR;
  }
  char read;
  while (stream >> read) {
    destination += read;
  }
  return CODE_OK;
}

/**
 * Reads integers from the indicated file into the provided array.
 *
 * Returns the number of integers read.
 */
size_t read_integers(const char *const filename, int *integer_array, const size_t integer_array_size) {
  FILE *file;
  size_t next_index = 0;
  file = fopen(filename, "r");
  if (file != nullptr) {
    while (next_index < integer_array_size && fscanf(file, "%d", &integer_array[next_index]) != EOF) {
      next_index++;
    }
    fclose(file);
  }
  return next_index; /* number of elements == index of next element */
}
