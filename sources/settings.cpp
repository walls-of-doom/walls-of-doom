#include "settings.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "logger.hpp"
#include "text.hpp"
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>

static const int SETTINGS_STRING_SIZE = 128;
static const int SETTINGS_BUFFER_SIZE = 4 * 1024;

static const int LOG_UNUSED_KEY_STRING_SIZE = SETTINGS_STRING_SIZE + 64;

static const char COMMENT_SYMBOL = '#';

class Limits {
public:
  long minimum = 0;
  long maximum = 0;
  long fallback = 0;
};

class DoubleLimits {
public:
  double minimum = 0;
  double maximum = 0;
  double fallback = 0;
};

static bool hide_cursor = true;

static F64 screen_occupancy = 0.8;

/* How many spaces should be left from the margins when printing text. */
static const int padding = 2;

static RepositionAlgorithm reposition_algorithm = REPOSITION_SELECT_AWARELY;

static const int MAXIMUM_FONT_SIZE = 48;
static const int MINIMUM_FONT_SIZE = 12;
static int font_size = 20;

static const long MINIMUM_PLATFORM_COUNT = 0;
static U64 platform_count = 16;

/* SDL has a limit at 16384. */
static const long MAXIMUM_DIMENSION = 16384;

static const S32 perk_interval = 20;
static const S32 perk_screen_duration = 10;
static const S32 perk_player_duration = 5;

static const long MAXIMUM_TILE_DIMENSION = 16384;
static const long MINIMUM_TILE_DIMENSION = 1;

static U16 tiles_on_x = 0;
static U16 tiles_on_y = 0;

static bool computed_window_size = false;

static int tile_w = -1;
static int tile_h = -1;

static int bar_height = -1;

static bool player_stops_platforms = false;

static int joystick_profile = JOYSTICK_PROFILE_DUALSHOCK;

static RendererType renderer_type = RENDERER_HARDWARE;

static int platform_max_width = 16;
static int platform_min_width = 4;

static int platform_max_speed = 4;
static int platform_min_speed = 1;

static int logging_player_score = 0;

static bool is_word_part(char character) { return (isspace(character) == 0) && character != '='; }

static void skip_to_word(const char **input) {
  while (**input != '\0' && !is_word_part(**input)) {
    (*input)++;
  }
}

static void copy_word(const char **input, char *destination) {
  size_t free_bytes = SETTINGS_STRING_SIZE;
  while (**input != '\0' && (static_cast<int>(is_word_part(**input)) != 0) && free_bytes > 1) {
    *destination++ = *(*input)++;
    free_bytes--;
  }
  *destination = '\0';
  free_bytes--;
  /* Discard the rest of the word, if it wasn't completely copied. */
  if (free_bytes == 0) {
    while (**input != '\0' && (static_cast<int>(is_word_part(**input)) != 0)) {
      (*input)++;
    }
  }
}

static void parse_word(const char **input, char *destination) {
  skip_to_word(input);
  copy_word(input, destination);
}

static bool is_comment_start(const char character) { return character == COMMENT_SYMBOL; }

static void skip_comments(const char **input) {
  while ((isspace(**input) != 0) || is_comment_start(**input)) {
    if (static_cast<int>(is_comment_start(**input)) != 0) {
      /* Skip to the next line. */
      while (**input != '\n' && **input != '\0') {
        (*input)++;
      }
    }
    /* Skip to the next token. */
    while (isspace(**input) != 0) {
      (*input)++;
    }
  }
}

static bool parse_line(const char **input, char *key, char *value) {
  skip_comments(input);
  parse_word(input, key);
  parse_word(input, value);
  return *key != '\0' && *value != '\0';
}

static double parse_double(const char *value, DoubleLimits limits) {
  double number;
  errno = 0;
  number = strtod(value, nullptr);
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

static long parse_integer(const char *value, Limits limits) {
  long integer;
  errno = 0;
  integer = strtol(value, nullptr, 10);
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

static bool parse_boolean(std::string value) {
  if (value == "true") {
    return true;
  } else if (value == "false") {
    return false;
  }
  throw std::domain_error("Invalid string.");
}

static ColorPair parse_color(const char *string) { return color_pair_from_string(string); }

static void log_unused_key(const char *key) {
  char log_buffer[LOG_UNUSED_KEY_STRING_SIZE];
  sprintf(log_buffer, "Unused settings key: %s.", key);
  log_message(log_buffer);
}

static void validate_settings() {
  if (get_window_width() > MAXIMUM_DIMENSION) {
    throw std::runtime_error("Window is too wide.");
  }
  if (get_window_height() > MAXIMUM_DIMENSION) {
    throw std::runtime_error("Window is too tall.");
  }
  if ((get_window_width() % get_tile_w()) != 0) {
    throw std::runtime_error("Window width is not divisible by tile width.");
  }
  if (((get_window_height() - 2 * get_bar_height()) % get_tile_h()) != 0) {
    throw std::runtime_error("Window height is not divisible by tile height.");
  }
}

void initialize_settings() {
  char input[SETTINGS_BUFFER_SIZE];
  char key[SETTINGS_STRING_SIZE];
  char value[SETTINGS_STRING_SIZE];
  const char *read = input;
  Limits limits{};
  DoubleLimits double_limits{};
  read_characters(SETTINGS_FILE, input, SETTINGS_BUFFER_SIZE);
  while (static_cast<int>(parse_line(&read, key, value)) != 0) {
    if (string_equals(key, "REPOSITION_ALGORITHM")) {
      if (string_equals(value, "REPOSITION_SELECT_BLINDLY")) {
        reposition_algorithm = REPOSITION_SELECT_BLINDLY;
        log_message("Set the reposition algorithm to SELECT_BLINDLY.");
      } else if (string_equals(value, "REPOSITION_SELECT_AWARELY")) {
        reposition_algorithm = REPOSITION_SELECT_AWARELY;
        log_message("Set the reposition algorithm to SELECT_AWARELY.");
      }
      /* Did not match any existing algorithm, do not change the default. */
    } else if (string_equals(key, "PLATFORM_COUNT")) {
      limits.minimum = MINIMUM_PLATFORM_COUNT;
      limits.maximum = MAXIMUM_PLATFORM_COUNT;
      limits.fallback = platform_count;
      platform_count = parse_integer(value, limits);
    } else if (string_equals(key, "FONT_SIZE")) {
      limits.minimum = MINIMUM_FONT_SIZE;
      limits.maximum = MAXIMUM_FONT_SIZE;
      limits.fallback = font_size;
      font_size = parse_integer(value, limits);
    } else if (string_equals(key, "TILES_ON_X")) {
      limits.minimum = 0;
      limits.maximum = std::numeric_limits<U16>::max();
      limits.fallback = 0;
      tiles_on_x = parse_integer(value, limits);
    } else if (string_equals(key, "TILES_ON_Y")) {
      limits.minimum = 0;
      limits.maximum = std::numeric_limits<U16>::max();
      limits.fallback = 0;
      tiles_on_y = parse_integer(value, limits);
    } else if (string_equals(key, "BAR_HEIGHT")) {
      limits.minimum = MINIMUM_TILE_DIMENSION;
      limits.maximum = MAXIMUM_TILE_DIMENSION;
      limits.fallback = bar_height;
      bar_height = parse_integer(value, limits);
    } else if (string_equals(key, "COLOR_PAIR_DEFAULT")) {
      COLOR_PAIR_DEFAULT = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PERK")) {
      COLOR_PAIR_PERK = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PLAYER")) {
      COLOR_PAIR_PLAYER = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_TOP_BAR")) {
      COLOR_PAIR_TOP_BAR = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_BOTTOM_BAR")) {
      COLOR_PAIR_BOTTOM_BAR = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PLATFORM_A")) {
      COLOR_PAIR_PLATFORM_A = parse_color(value);
    } else if (string_equals(key, "COLOR_PAIR_PLATFORM_B")) {
      COLOR_PAIR_PLATFORM_B = parse_color(value);
    } else if (string_equals(key, "PLAYER_STOPS_PLATFORMS")) {
      player_stops_platforms = parse_boolean(value);
    } else if (string_equals(key, "LOGGING_PLAYER_SCORE")) {
      logging_player_score = parse_boolean(value);
    } else if (string_equals(key, "JOYSTICK_PROFILE")) {
      if (string_equals(value, "XBOX")) {
        joystick_profile = JOYSTICK_PROFILE_XBOX;
      } else if (string_equals(value, "DUALSHOCK")) {
        joystick_profile = JOYSTICK_PROFILE_DUALSHOCK;
      }
    } else if (string_equals(key, "PLATFORM_MAXIMUM_WIDTH")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_max_width;
      platform_max_width = parse_integer(value, limits);
    } else if (string_equals(key, "PLATFORM_MINIMUM_WIDTH")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_min_width;
      platform_min_width = parse_integer(value, limits);
    } else if (string_equals(key, "PLATFORM_MAXIMUM_SPEED")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_max_speed;
      platform_max_speed = parse_integer(value, limits);
    } else if (string_equals(key, "PLATFORM_MINIMUM_SPEED")) {
      limits.minimum = 1;
      limits.maximum = 65535;
      limits.fallback = platform_min_speed;
      platform_min_speed = parse_integer(value, limits);
    } else if (string_equals(key, "SCREEN_OCCUPANCY")) {
      double_limits.minimum = 0.1;
      double_limits.maximum = 1.0;
      double_limits.fallback = screen_occupancy;
      screen_occupancy = parse_double(value, double_limits);
    } else if (string_equals(key, "HIDE_CURSOR")) {
      hide_cursor = parse_boolean(value);
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
  validate_settings();
}

void compute_window_size() {
  SDL_DisplayMode display_mode{};
  SDL_GetCurrentDisplayMode(0, &display_mode);
  const auto w = display_mode.w;
  const auto h = display_mode.h;
  tile_w = 0;
  while (tiles_on_x * (tile_w + 1) < get_screen_occupancy() * w) {
    tile_w++;
  }
  tile_h = 0;
  while (tiles_on_y * (tile_h + 1) + get_bar_height() * 2 < get_screen_occupancy() * h) {
    tile_h++;
  }
  log_message("Using " + std::to_string(tile_w) + "x" + std::to_string(tile_h) + " for tile size.");
  computed_window_size = true;
}

RepositionAlgorithm get_reposition_algorithm() { return reposition_algorithm; }

RendererType get_renderer_type() { return renderer_type; }

U64 get_platform_count() { return platform_count; }

int get_font_size() { return font_size; }

U16 get_tiles_on_x() { return tiles_on_x; }

U16 get_tiles_on_y() { return tiles_on_y; }

int get_tile_w() {
  if (!computed_window_size) {
    compute_window_size();
  }
  return tile_w;
}

int get_tile_h() {
  if (!computed_window_size) {
    compute_window_size();
  }
  return tile_h;
}

int get_bar_height() { return bar_height; }

int get_window_width() { return get_tile_w() * get_tiles_on_x(); }

int get_window_height() { return get_tile_h() * get_tiles_on_y() + get_bar_height() * 2; }

long get_padding() { return padding; }

bool get_player_stops_platforms() { return player_stops_platforms; }

int get_joystick_profile() { return joystick_profile; }

int get_platform_max_width() { return platform_max_width; }

int get_platform_min_width() { return platform_min_width; }

int get_platform_max_speed() { return platform_max_speed; }

int get_platform_min_speed() { return platform_min_speed; }

int is_logging_player_score() { return logging_player_score; }

S32 get_perk_interval() { return perk_interval; }

S32 get_perk_screen_duration() { return perk_screen_duration; }

S32 get_perk_player_duration() { return perk_player_duration; }

bool should_hide_cursor() { return hide_cursor; }

F64 get_screen_occupancy() { return screen_occupancy; }
