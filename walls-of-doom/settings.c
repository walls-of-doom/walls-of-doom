#include "settings.h"

#include "constants.h"
#include "data.h"
#include "logger.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define SETTINGS_STRING_SIZE 64
#define SETTINGS_BUFFER_SIZE 4096

#define MINIMUM_PLATFORM_COUNT 0
#define DEFAULT_PLATFORM_COUNT 16

static RepositionAlgorithm reposition_algorithm = REPOSITION_SELECT_AWARELY;
static long platform_count = DEFAULT_PLATFORM_COUNT;

static int parse_line(const char **input, char *key, char *value) {
  int reading_key = 1;
  size_t key_size = 0;
  size_t value_size = 0;
  const char *buffer = *input;
  while (*buffer != '\0' && *buffer != '\n') {
    if (reading_key) {
      if (*buffer == '=') {
        reading_key = 0;
      } else if (key_size + 1 == SETTINGS_STRING_SIZE) {
        reading_key = 0;
      } else {
        *key++ = *buffer;
        key_size++;
      }
    } else {
      if (value_size + 1 == SETTINGS_STRING_SIZE) {
        break;
      } else {
        *value++ = *buffer;
        value_size++;
      }
    }
    buffer++;
  }
  /* Skip trailing newlines. */
  while (*buffer == '\n') {
    buffer++;
  }
  *key = '\0';
  *value = '\0';
  *input = buffer;
  if (key_size == 0 && value_size == 0) {
    return 0;
  }
  return 1;
}

void initialize_settings(void) {
  char input[SETTINGS_BUFFER_SIZE];
  char key[SETTINGS_STRING_SIZE];
  char value[SETTINGS_STRING_SIZE];
  const char *read = input;
  read_characters(SETTINGS_FILE, input, SETTINGS_BUFFER_SIZE);
  while (parse_line(&read, key, value)) {
    if (strcmp(key, "REPOSITION_ALGORITHM") == 0) {
      if (strcmp(value, "REPOSITION_SELECT_BLINDLY") == 0) {
        reposition_algorithm = REPOSITION_SELECT_BLINDLY;
        log_message("Set the reposition algorithm to SELECT_BLINDLY");
      } else if (strcmp(value, "REPOSITION_SELECT_AWARELY") == 0) {
        reposition_algorithm = REPOSITION_SELECT_AWARELY;
        log_message("Set the reposition algorithm to SELECT_AWARELY");
      }
      /* Did not match any existing algorithm, do not change the default. */
    } else if (strcmp(key, "PLATFORM_COUNT") == 0) {
      platform_count = strtol(value, NULL, 10);
      if (errno) {
        log_message("Failed to read an integer for PLATFORM_COUNT");
        errno = 0;
      }
      if (platform_count < MINIMUM_PLATFORM_COUNT) {
        platform_count = MINIMUM_PLATFORM_COUNT;
      } else if (platform_count > MAXIMUM_PLATFORM_COUNT) {
        platform_count = MAXIMUM_PLATFORM_COUNT;
      }
    }
  }
}

RepositionAlgorithm get_reposition_algorithm(void) {
  return reposition_algorithm;
}

long get_platform_count(void) { return platform_count; }
