#ifndef SETTINGS_H
#define SETTINGS_H

#include "integers.hpp"
#include <string>

#define MAXIMUM_PLATFORM_COUNT 256

extern const char *const settings_filename;

enum RendererType { RENDERER_HARDWARE, RENDERER_SOFTWARE };

enum JoystickProfile { JOYSTICK_PROFILE_XBOX, JOYSTICK_PROFILE_DUALSHOCK };

enum RepositionAlgorithm { REPOSITION_SELECT_BLINDLY, REPOSITION_SELECT_AWARELY };

class Settings {
public:
  explicit Settings(const std::string &filename);

  inline RendererType get_renderer_type() const {
    return renderer_type;
  }

  inline JoystickProfile get_joystick_profile() const {
    return joystick_profile;
  }

  inline RepositionAlgorithm get_reposition_algorithm() const {
    return reposition_algorithm;
  }

  inline bool get_hide_cursor() const {
    return hide_cursor;
  }

  inline bool get_player_stops_platforms() const {
    return player_stops_platforms;
  }

  inline bool is_logging_player_score() const {
    return logging_player_score;
  }

  inline F32 get_screen_occupancy() const {
    return screen_occupancy;
  }

  inline U32 get_platform_count() const {
    return platform_count;
  }

  inline U32 get_font_size() const {
    return font_size;
  }

  // How many spaces should be left from the margins when printing text.
  inline U32 get_padding() const {
    return padding;
  }

  inline U32 get_perk_interval() const {
    return perk_interval;
  }

  inline U32 get_perk_screen_duration() const {
    return perk_screen_duration;
  };

  inline U32 get_perk_player_duration() const {
    return perk_player_duration;
  };

  inline U32 get_window_width() const {
    return get_tile_w() * get_tiles_on_x();
  }

  inline U32 get_window_height() const {
    return get_tile_h() * get_tiles_on_y() + get_bar_height() * 2;
  };

  inline U32 get_tiles_on_x() const {
    return tiles_on_x;
  }

  inline U32 get_tiles_on_y() const {
    return tiles_on_y;
  }

  inline U32 get_tile_w() const {
    return tile_w;
  }

  inline U32 get_tile_h() const {
    return tile_h;
  }

  inline U32 get_bar_height() const {
    return bar_height;
  }

  inline U32 get_platform_max_width() const {
    return platform_max_width;
  }

  inline U32 get_platform_min_width() const {
    return platform_min_width;
  }

  inline U32 get_platform_max_speed() const {
    return platform_max_speed;
  }

  inline U32 get_platform_min_speed() const {
    return platform_min_speed;
  }

  void compute_window_size();

  void validate_settings() const;

private:
  bool computed_window_size = false;

  RendererType renderer_type = RENDERER_HARDWARE;

  JoystickProfile joystick_profile = JOYSTICK_PROFILE_DUALSHOCK;

  RepositionAlgorithm reposition_algorithm = REPOSITION_SELECT_AWARELY;

  bool hide_cursor = true;
  bool player_stops_platforms = false;
  bool logging_player_score = false;

  F32 screen_occupancy = 0.8;

  U32 platform_count = 16;

  U32 font_size = 20;

  U32 padding = 2;

  U32 perk_interval = 20;
  U32 perk_screen_duration = 10;
  U32 perk_player_duration = 5;

  U32 tiles_on_x = 0;
  U32 tiles_on_y = 0;

  U32 tile_w = 0;
  U32 tile_h = 0;

  U32 bar_height = 0;

  U32 platform_min_width = 4;
  U32 platform_max_width = 16;

  U32 platform_min_speed = 1;
  U32 platform_max_speed = 4;
};

#endif
