#include "settings.h"
#include "color.h"
#include "constants.h"
#include "data.h"
#include "logger.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define SETTINGS_STRING_SIZE 128
#define SETTINGS_BUFFER_SIZE 4096

#define LOG_UNUSED_KEY_STRING_SIZE 64 + SETTINGS_STRING_SIZE

#define DEFAULT_PLATFORM_COUNT 16
#define MINIMUM_PLATFORM_COUNT 0

#define MAXIMUM_FONT_SIZE 48
#define DEFAULT_FONT_SIZE 20
#define MINIMUM_FONT_SIZE 12

#define DEFAULT_COLUMNS 80
#define MINIMUM_COLUMNS 40

#define DEFAULT_LINES 30
#define MINIMUM_LINES 20

#define DEFAULT_PLAYER_STOPS_PLATFORMS 0

#define MAXIMUM_INVESTMENT_AMOUNT 12000
#define DEFAULT_INVESTMENT_AMOUNT 60
#define MINIMUM_INVESTMENT_AMOUNT 1

/* The period of the investment, in seconds .*/
#define MAXIMUM_INVESTMENT_PERIOD 300
#define DEFAULT_INVESTMENT_PERIOD 15
#define MINIMUM_INVESTMENT_PERIOD 1

#define MAXIMUM_INVESTMENT_MAXIMUM_FACTOR 1000
#define DEFAULT_INVESTMENT_MAXIMUM_FACTOR 140
#define MINIMUM_INVESTMENT_MAXIMUM_FACTOR 100

#define MAXIMUM_INVESTMENT_MINIMUM_FACTOR 1000
#define DEFAULT_INVESTMENT_MINIMUM_FACTOR 90
#define MINIMUM_INVESTMENT_MINIMUM_FACTOR 0

/**
 * How many spaces should be left from the margins when printing text.
 */
#define DEFAULT_PADDING 2

static RepositionAlgorithm reposition_algorithm = REPOSITION_SELECT_AWARELY;
static long platform_count = DEFAULT_PLATFORM_COUNT;
static int font_size = DEFAULT_FONT_SIZE;
static long columns = DEFAULT_COLUMNS;
static long lines = DEFAULT_LINES;
static long padding = DEFAULT_PADDING;
static int player_stops_platforms = DEFAULT_PLAYER_STOPS_PLATFORMS;
static int investment_amount = DEFAULT_INVESTMENT_AMOUNT;
static int investment_period = DEFAULT_INVESTMENT_PERIOD;
static int investment_maximum_factor = DEFAULT_INVESTMENT_MAXIMUM_FACTOR;
static int investment_minimum_factor = DEFAULT_INVESTMENT_MINIMUM_FACTOR;

static int is_word_part(char character) {
  return !isspace(character) && character != '=';
}

static void skip_to_word(const char **input) {
  while (**input != '\0' && !is_word_part(**input)) {
    (*input)++;
  }
}

static void copy_word(const char **input, char *destination) {
  while (**input != '\0' && is_word_part(**input)) {
    *destination++ = *(*input)++;
  }
  *destination = '\0';
}

static void parse_word(const char **input, char *destination) {
  skip_to_word(input);
  copy_word(input, destination);
}

static int parse_line(const char **input, char *key, char *value) {
  parse_word(input, key);
  parse_word(input, value);
  return *key != '\0' && *value != '\0';
}

struct Limits {
  long minimum;
  long maximum;
  long fallback;
};

static long parse_value(const char *value, struct Limits limits) {
  long integer;
  errno = 0;
  integer = strtol(value, NULL, 10);
  if (errno) {
    log_message("Failed to read integer from input string!");
    errno = 0;
    return limits.fallback;
  }
  if (integer < limits.minimum) {
    return limits.minimum;
  }
  if (integer > limits.maximum) {
    return limits.maximum;
  }
  return integer;
}

static int parse_boolean(const char *value, const int fallback) {
  /* 0, 1, and fallback are all safe to cast to int. */
  struct Limits boolean_limits;
  boolean_limits.minimum = 0;
  boolean_limits.maximum = 1;
  boolean_limits.fallback = fallback;
  return (int)parse_value(value, boolean_limits);
}

static ColorPair parse_color(const char *string) {
  return color_pair_from_string(string);
}

static void log_unused_key(const char *key) {
  char log_buffer[LOG_UNUSED_KEY_STRING_SIZE];
  sprintf(log_buffer, "Unused settings key: %s", key);
  log_message(log_buffer);
}

void initialize_settings(void) {
  char input[SETTINGS_BUFFER_SIZE];
  char key[SETTINGS_STRING_SIZE];
  char value[SETTINGS_STRING_SIZE];
  const char *read = input;
  struct Limits limits;
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
      limits.minimum = MINIMUM_PLATFORM_COUNT;
      limits.maximum = MAXIMUM_PLATFORM_COUNT;
      limits.fallback = DEFAULT_PLATFORM_COUNT;
      platform_count = parse_value(value, limits);
    } else if (strcmp(key, "FONT_SIZE") == 0) {
      limits.minimum = MINIMUM_FONT_SIZE;
      limits.maximum = MAXIMUM_FONT_SIZE;
      limits.fallback = DEFAULT_FONT_SIZE;
      font_size = parse_value(value, limits);
    } else if (strcmp(key, "COLUMNS") == 0) {
      limits.minimum = MINIMUM_COLUMNS;
      limits.maximum = MAXIMUM_COLUMNS;
      limits.fallback = DEFAULT_COLUMNS;
      columns = parse_value(value, limits);
    } else if (strcmp(key, "LINES") == 0) {
      limits.minimum = MINIMUM_LINES;
      limits.maximum = MAXIMUM_LINES;
      limits.fallback = DEFAULT_LINES;
      lines = parse_value(value, limits);
    } else if (strcmp(key, "COLOR_PAIR_DEFAULT") == 0) {
      COLOR_PAIR_DEFAULT = parse_color(value);
    } else if (strcmp(key, "COLOR_PAIR_PERK") == 0) {
      COLOR_PAIR_PERK = parse_color(value);
    } else if (strcmp(key, "COLOR_PAIR_PLAYER") == 0) {
      COLOR_PAIR_PLAYER = parse_color(value);
    } else if (strcmp(key, "COLOR_PAIR_TOP_BAR") == 0) {
      COLOR_PAIR_TOP_BAR = parse_color(value);
    } else if (strcmp(key, "COLOR_PAIR_PLATFORM") == 0) {
      COLOR_PAIR_PLATFORM = parse_color(value);
    } else if (strcmp(key, "COLOR_PAIR_BOTTOM_BAR") == 0) {
      COLOR_PAIR_BOTTOM_BAR = parse_color(value);
    } else if (strcmp(key, "PLAYER_STOPS_PLATFORMS") == 0) {
      limits.fallback = DEFAULT_PLAYER_STOPS_PLATFORMS;
      player_stops_platforms = parse_boolean(value, limits.fallback);
    } else if (strcmp(key, "INVESTMENT_AMOUNT") == 0) {
      limits.minimum = MINIMUM_INVESTMENT_AMOUNT;
      limits.maximum = MAXIMUM_INVESTMENT_AMOUNT;
      limits.fallback = DEFAULT_INVESTMENT_AMOUNT;
      investment_amount = parse_value(value, limits);
    } else if (strcmp(key, "INVESTMENT_PERIOD") == 0) {
      limits.minimum = MINIMUM_INVESTMENT_PERIOD;
      limits.maximum = MAXIMUM_INVESTMENT_PERIOD;
      limits.fallback = DEFAULT_INVESTMENT_PERIOD;
      investment_period = parse_value(value, limits);
    } else if (strcmp(key, "INVESTMENT_MAXIMUM_FACTOR") == 0) {
      limits.minimum = MINIMUM_INVESTMENT_MAXIMUM_FACTOR;
      limits.maximum = MAXIMUM_INVESTMENT_MAXIMUM_FACTOR;
      limits.fallback = DEFAULT_INVESTMENT_MAXIMUM_FACTOR;
      investment_maximum_factor = parse_value(value, limits);
    } else if (strcmp(key, "INVESTMENT_MINIMUM_FACTOR") == 0) {
      limits.minimum = MINIMUM_INVESTMENT_MINIMUM_FACTOR;
      limits.maximum = MAXIMUM_INVESTMENT_MINIMUM_FACTOR;
      limits.fallback = DEFAULT_INVESTMENT_MINIMUM_FACTOR;
      investment_minimum_factor = parse_value(value, limits);
    } else {
      log_unused_key(key);
    }
  }
}
RepositionAlgorithm get_reposition_algorithm(void) {
  return reposition_algorithm;
}

long get_platform_count(void) { return platform_count; }

int get_font_size(void) { return font_size; }

long get_columns(void) { return columns; }

long get_lines(void) { return lines; }

long get_padding(void) { return padding; }

int get_player_stops_platforms(void) { return player_stops_platforms; }

int get_investment_amount(void) { return investment_amount; }

int get_investment_period(void) { return investment_period; }

int get_investment_maximum_factor(void) { return investment_maximum_factor; }

int get_investment_minimum_factor(void) { return investment_minimum_factor; }
