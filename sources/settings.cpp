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
#include <sstream>
#include <stdexcept>

const char *const settings_filename = "assets/settings/settings.txt";

static const int SETTINGS_STRING_SIZE = 128;
static const int SETTINGS_BUFFER_SIZE = 4 * 1024;

static const int LOG_UNUSED_KEY_STRING_SIZE = SETTINGS_STRING_SIZE + 64;

static const char COMMENT_SYMBOL = '#';

static const U32 MAXIMUM_FONT_SIZE = 48;
static const U32 MINIMUM_FONT_SIZE = 12;

static const U32 MINIMUM_PLATFORM_COUNT = 0;

/* SDL has a limit at 16384. */
static const U32 MAXIMUM_DIMENSION = 16384;

static bool is_word_part(char character) {
  return (isspace(character) == 0) && character != '=';
}

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

static bool is_comment_start(const char character) {
  return character == COMMENT_SYMBOL;
}

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

template <typename T> static T parse(const std::string &text) {
  T value;
  std::stringstream stream;
  stream << text;
  stream >> value;
  if (stream.fail()) {
    std::domain_error("Cannot parse '" + text + "'.");
  }
  return value;
}

template <typename T> static T parse(const std::string &text, T minimum, T maximum) {
  if (minimum > maximum) {
    throw std::logic_error("Minimum is greater than maximum.");
  }
  T value;
  std::stringstream stream;
  stream << text;
  stream >> value;
  if (stream.fail()) {
    std::domain_error("Cannot parse '" + text + "'.");
  }
  return std::max(minimum, std::min(maximum, value));
}

static bool parse_boolean(const std::string &value) {
  if (value == "true") {
    return true;
  } else if (value == "false") {
    return false;
  }
  throw std::domain_error("Input is not convertible to boolean.");
}

static void log_unused_key(const char *key) {
  char log_buffer[LOG_UNUSED_KEY_STRING_SIZE];
  sprintf(log_buffer, "Unused settings key: %s.", key);
  log_message(log_buffer);
}

Settings::Settings(const std::string &filename) {
  char input[SETTINGS_BUFFER_SIZE];
  char key[SETTINGS_STRING_SIZE];
  char value[SETTINGS_STRING_SIZE];
  const char *read = input;
  read_characters(filename.c_str(), input, SETTINGS_BUFFER_SIZE);
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
      platform_count = parse<decltype(platform_count)>(value, MINIMUM_PLATFORM_COUNT, MAXIMUM_PLATFORM_COUNT);
    } else if (string_equals(key, "FONT_SIZE")) {
      font_size = parse(value, MINIMUM_FONT_SIZE, MAXIMUM_FONT_SIZE);
    } else if (string_equals(key, "TILES_ON_X")) {
      tiles_on_x = parse<decltype(tiles_on_x)>(value);
    } else if (string_equals(key, "TILES_ON_Y")) {
      tiles_on_y = parse<decltype(tiles_on_y)>(value);
    } else if (string_equals(key, "BAR_HEIGHT")) {
      bar_height = parse<decltype(bar_height)>(value);
    } else if (string_equals(key, "COLOR_PAIR_DEFAULT")) {
      COLOR_PAIR_DEFAULT = color_pair_from_string(value);
    } else if (string_equals(key, "COLOR_PAIR_PERK")) {
      COLOR_PAIR_PERK = color_pair_from_string(value);
    } else if (string_equals(key, "COLOR_PAIR_PLAYER")) {
      COLOR_PAIR_PLAYER = color_pair_from_string(value);
    } else if (string_equals(key, "COLOR_PAIR_TOP_BAR")) {
      COLOR_PAIR_TOP_BAR = color_pair_from_string(value);
    } else if (string_equals(key, "COLOR_PAIR_BOTTOM_BAR")) {
      COLOR_PAIR_BOTTOM_BAR = color_pair_from_string(value);
    } else if (string_equals(key, "COLOR_PAIR_PLATFORM_A")) {
      COLOR_PAIR_PLATFORM_A = color_pair_from_string(value);
    } else if (string_equals(key, "COLOR_PAIR_PLATFORM_B")) {
      COLOR_PAIR_PLATFORM_B = color_pair_from_string(value);
    } else if (string_equals(key, "PLAYER_STOPS_PLATFORMS")) {
      player_stops_platforms = parse_boolean(value);
    } else if (string_equals(key, "LOGGING_PLAYER_SCORE")) {
      logging_player_score = parse_boolean(value);
    } else if (string_equals(key, "JOYSTICK_PROFILE")) {
      if (string_equals(value, "XBOX")) {
        joystick_profile = JOYSTICK_PROFILE_XBOX;
      } else if (string_equals(value, "DUALSHOCK")) {
        joystick_profile = JOYSTICK_PROFILE_DUALSHOCK;
      } else {
        throw std::domain_error("Invalid value for JOYSTICK_PROFILE.");
      }
    } else if (string_equals(key, "PLATFORM_MAXIMUM_WIDTH")) {
      platform_max_width = parse<decltype(platform_max_width)>(value);
    } else if (string_equals(key, "PLATFORM_MINIMUM_WIDTH")) {
      platform_min_width = parse<decltype(platform_min_width)>(value);
    } else if (string_equals(key, "PLATFORM_MAXIMUM_SPEED")) {
      platform_max_speed = parse<decltype(platform_max_speed)>(value);
    } else if (string_equals(key, "PLATFORM_MINIMUM_SPEED")) {
      platform_min_speed = parse<decltype(platform_min_speed)>(value);
    } else if (string_equals(key, "SCREEN_OCCUPANCY")) {
      screen_occupancy = parse(value, 0.1f, 1.0f);
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
}

void Settings::compute_window_size() {
  if (computed_window_size) {
    throw std::logic_error("Double initialization.");
  }
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

void Settings::validate_settings() const {
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
  if (get_platform_min_speed() > get_platform_max_speed()) {
    throw std::runtime_error("Minimum is greater than maximum.");
  }
  if (get_platform_min_width() > get_platform_max_width()) {
    throw std::runtime_error("Minimum is greater than maximum.");
  }
}
