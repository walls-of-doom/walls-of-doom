#include "settings.h"
#include "color.h"
#include "constants.h"
#include "data.h"
#include "investment.h"
#include "logger.h"
#include "text.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define SETTINGS_STRING_SIZE 128
#define SETTINGS_BUFFER_SIZE 4096

#define LOG_UNUSED_KEY_STRING_SIZE 64 + SETTINGS_STRING_SIZE

#define COMMENT_SYMBOL '#'

/* The use of variables is preferred over symbolic constants, when possible. */

/* How many spaces should be left from the margins when printing text. */
static const int padding = 2;

static RepositionAlgorithm reposition_algorithm = REPOSITION_SELECT_AWARELY;

static const int MAXIMUM_FONT_SIZE = 48;
static const int MINIMUM_FONT_SIZE = 12;
static int font_size = 20;

static const long MINIMUM_PLATFORM_COUNT = 0;
static long platform_count = 16;

static const long MINIMUM_COLUMNS = 40;
static long columns = 80;

static const long MINIMUM_LINES = 20;
static long lines = 30;

/* SDL has a limit at 16384. */
static const long MAXIMUM_DIMENSION = 16384;
static const long MINIMUM_DIMENSION = 240;
static int width = -1;
static int height = -1;

static int player_stops_platforms = 0;

static int joystick_profile = JOYSTICK_PROFILE_DUALSHOCK;

static InvestmentMode investment_mode = INVESTMENT_MODE_PROPORTIONAL;

/* The period of the investment, in seconds .*/
static const long MAXIMUM_INVESTMENT_PERIOD = 300;
static const long MINIMUM_INVESTMENT_PERIOD = 1;
static long investment_period = 15;

static const long MAXIMUM_INVESTMENT_AMOUNT = 12000;
static const long MINIMUM_INVESTMENT_AMOUNT = 1;
static long investment_amount = 60;

static const double MAXIMUM_INVESTMENT_PROPORTION = 1.0;
static const double MINIMUM_INVESTMENT_PROPORTION = 0.1;
static double investment_proportion = 60;

static const double MAXIMUM_INVESTMENT_MAXIMUM_FACTOR = 5.0;
static const double MINIMUM_INVESTMENT_MAXIMUM_FACTOR = 1.0;
static double investment_maximum_factor = 1.50;

static const double MAXIMUM_INVESTMENT_MINIMUM_FACTOR = 0.0;
static const double MINIMUM_INVESTMENT_MINIMUM_FACTOR = 1.0;
static double investment_minimum_factor = 0.75;

static RendererType renderer_type = RENDERER_HARDWARE;

static int platform_max_width = 16;
static int platform_min_width = 4;

static int platform_max_speed = 4;
static int platform_min_speed = 1;

static int logging_player_score = 0;

static int is_word_part(char character) {
  return !isspace(character) && character != '=';
}

static void skip_to_word(const char **input) {
  while (**input != '\0' && !is_word_part(**input)) {
    (*input)++;
  }
}

static void copy_word(const char **input, char *destination) {
  size_t free_bytes = SETTINGS_STRING_SIZE;
  while (**input != '\0' && is_word_part(**input) && free_bytes > 1) {
    *destination++ = *(*input)++;
    free_bytes--;
  }
  *destination = '\0';
  free_bytes--;
  /* Discard the rest of the word, if it wasn't completely copied. */
  if (free_bytes == 0) {
    while (**input != '\0' && is_word_part(**input)) {
      (*input)++;
    }
  }
}

static void parse_word(const char **input, char *destination) {
  skip_to_word(input);
  copy_word(input, destination);
}

static int is_comment_start(const char character) {
  return character == COMMENT_SYMBOL;
}

static void skip_comments(const char **input) {
  while (isspace(**input) || is_comment_start(**input)) {
    if (is_comment_start(**input)) {
      /* Skip to the next line. */
      while (**input != '\n' && **input != '\0') {
        (*input)++;
      }
    }
    /* Skip to the next token. */
    while (isspace(**input)) {
      (*input)++;
    }
  }
}

static int parse_line(const char **input, char *key, char *value) {
  skip_comments(input);
  parse_word(input, key);
  parse_word(input, value);
  return *key != '\0' && *value != '\0';
}

struct Limits {
  long minimum;
  long maximum;
  long fallback;
};

struct DoubleLimits {
  double minimum;
  double maximum;
  double fallback;
};

static double parse_double(const char *value, struct DoubleLimits limits) {
  double number;
  errno = 0;
  number = strtod(value, NULL);
  if (errno) {
    log_message("Failed to read double from input string!");
    errno = 0;
    return limits.fallback;
  }
  if (number < limits.minimum) {
    return limits.minimum;
  }
  if (number > limits.maximum) {
    return limits.maximum;
  }
  return number;
}

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
  struct DoubleLimits double_limits;
  int i;
  read_characters(SETTINGS_FILE, input, SETTINGS_BUFFER_SIZE);
  while (parse_line(&read, key, value)) {
    if (string_equals(key, "REPOSITION_ALGORITHM")) {
      if (string_equals(value, "REPOSITION_SELECT_BLINDLY")) {
        reposition_algorithm = REPOSITION_SELECT_BLINDLY;
        log_message("Set the reposition algorithm to SELECT_BLINDLY");
      } else if (string_equals(value, "REPOSITION_SELECT_AWARELY")) {
        reposition_algorithm = REPOSITION_SELECT_AWARELY;
        log_message("Set the reposition algorithm to SELECT_AWARELY");
      }
      /* Did not match any existing algorithm, do not change the default. */
    } else if (string_equals(key, "PLATFORM_COUNT")) {
      limits.minimum = MINIMUM_PLATFORM_COUNT;
      limits.maximum = MAXIMUM_PLATFORM_COUNT;
      limits.fallback = platform_count;
      platform_count = parse_value(value, limits);
    } else if (string_equals(key, "FONT_SIZE")) {
      limits.minimum = MINIMUM_FONT_SIZE;
      limits.maximum = MAXIMUM_FONT_SIZE;
      limits.fallback = font_size;
      font_size = parse_value(value, limits);
    } else if (string_equals(key, "COLUMNS")) {
      limits.minimum = MINIMUM_COLUMNS;
      limits.maximum = MAXIMUM_COLUMNS;
      limits.fallback = columns;
      columns = parse_value(value, limits);
    } else if (string_equals(key, "LINES")) {
      limits.minimum = MINIMUM_LINES;
      limits.maximum = MAXIMUM_LINES;
      limits.fallback = lines;
      lines = parse_value(value, limits);
    } else if (string_equals(key, "WIDTH")) {
      limits.minimum = MINIMUM_DIMENSION;
      limits.maximum = MAXIMUM_DIMENSION;
      limits.fallback = width;
      width = parse_value(value, limits);
    } else if (string_equals(key, "HEIGHT")) {
      limits.minimum = MINIMUM_DIMENSION;
      limits.maximum = MAXIMUM_DIMENSION;
      limits.fallback = height;
      height = parse_value(value, limits);
    } else if (string_equals(key, "COLOR_PAIR_DEFAULT")) {
      COLOR_PAIR_DEFAULT = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PERK")) {
      COLOR_PAIR_PERK = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PLAYER")) {
      COLOR_PAIR_PLAYER = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_TOP_BAR")) {
      COLOR_PAIR_TOP_BAR = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PLATFORM")) {
      COLOR_PAIR_PLATFORM = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_BOTTOM_BAR")) {
      COLOR_PAIR_BOTTOM_BAR = parse_color(value);
    } else if (string_equals(key, "PLAYER_STOPS_PLATFORMS")) {
      limits.fallback = player_stops_platforms;
      player_stops_platforms = parse_boolean(value, limits.fallback);
    } else if (string_equals(key, "LOGGING_PLAYER_SCORE")) {
      limits.fallback = logging_player_score;
      logging_player_score = parse_boolean(value, limits.fallback);
    } else if (string_equals(key, "JOYSTICK_PROFILE")) {
      if (string_equals(value, "XBOX")) {
        joystick_profile = JOYSTICK_PROFILE_XBOX;
      } else if (string_equals(value, "DUALSHOCK")) {
        joystick_profile = JOYSTICK_PROFILE_DUALSHOCK;
      }
    } else if (string_equals(key, "INVESTMENT_MODE")) {
      for (i = 0; i < INVESTMENT_MODE_COUNT; ++i) {
        if (string_equals(value, get_investment_mode_name(i))) {
          investment_mode = i;
        }
      }
    } else if (string_equals(key, "INVESTMENT_AMOUNT")) {
      limits.minimum = MINIMUM_INVESTMENT_AMOUNT;
      limits.maximum = MAXIMUM_INVESTMENT_AMOUNT;
      limits.fallback = investment_amount;
      investment_amount = parse_value(value, limits);
    } else if (string_equals(key, "INVESTMENT_PROPORTION")) {
      double_limits.minimum = MINIMUM_INVESTMENT_PROPORTION;
      double_limits.maximum = MAXIMUM_INVESTMENT_PROPORTION;
      double_limits.fallback = investment_proportion;
      investment_proportion = parse_double(value, double_limits);
    } else if (string_equals(key, "INVESTMENT_PERIOD")) {
      limits.minimum = MINIMUM_INVESTMENT_PERIOD;
      limits.maximum = MAXIMUM_INVESTMENT_PERIOD;
      limits.fallback = investment_period;
      investment_period = parse_value(value, limits);
    } else if (string_equals(key, "INVESTMENT_MAXIMUM_FACTOR")) {
      double_limits.minimum = MINIMUM_INVESTMENT_MAXIMUM_FACTOR;
      double_limits.maximum = MAXIMUM_INVESTMENT_MAXIMUM_FACTOR;
      double_limits.fallback = investment_maximum_factor;
      investment_maximum_factor = parse_double(value, double_limits);
    } else if (string_equals(key, "INVESTMENT_MINIMUM_FACTOR")) {
      double_limits.minimum = MINIMUM_INVESTMENT_MINIMUM_FACTOR;
      double_limits.maximum = MAXIMUM_INVESTMENT_MINIMUM_FACTOR;
      double_limits.fallback = investment_minimum_factor;
      investment_minimum_factor = parse_double(value, double_limits);
    } else if (string_equals(key, "PLATFORM_MAXIMUM_WIDTH")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_max_width;
      platform_max_width = parse_value(value, limits);
    } else if (string_equals(key, "PLATFORM_MINIMUM_WIDTH")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_min_width;
      platform_min_width = parse_value(value, limits);
    } else if (string_equals(key, "PLATFORM_MAXIMUM_SPEED")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_max_speed;
      platform_max_speed = parse_value(value, limits);
    } else if (string_equals(key, "PLATFORM_MINIMUM_SPEED")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_min_speed;
      platform_min_speed = parse_value(value, limits);
    } else if (string_equals(key, "RENDERER_TYPE")) {
      if (string_equals(value, "HARDWARE")) {
        renderer_type = RENDERER_HARDWARE;
      } else {
        renderer_type = RENDERER_SOFTWARE;
      }
    } else {
      log_unused_key(key);
    }
  }
}

RepositionAlgorithm get_reposition_algorithm(void) {
  return reposition_algorithm;
}

RendererType get_renderer_type(void) { return renderer_type; }

long get_platform_count(void) { return platform_count; }

int get_font_size(void) { return font_size; }

long get_columns(void) { return columns; }

long get_lines(void) { return lines; }

int get_requested_window_width(void) { return width; }

int get_requested_window_height(void) { return height; }

long get_padding(void) { return padding; }

int get_player_stops_platforms(void) { return player_stops_platforms; }

int get_joystick_profile() { return joystick_profile; }

InvestmentMode get_investment_mode(void) { return investment_mode; }

int get_investment_amount(void) { return investment_amount; }

double get_investment_proportion(void) { return investment_proportion; }

int get_investment_period(void) { return investment_period; }

double get_investment_maximum_factor(void) { return investment_maximum_factor; }

double get_investment_minimum_factor(void) { return investment_minimum_factor; }

int get_platform_max_width(void) { return platform_max_width; }

int get_platform_min_width(void) { return platform_min_width; }

int get_platform_max_speed(void) { return platform_max_speed; }

int get_platform_min_speed(void) { return platform_min_speed; }

int is_logging_player_score(void) { return logging_player_score; }
