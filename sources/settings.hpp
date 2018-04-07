#ifndef SETTINGS_H
#define SETTINGS_H

#include "integers.hpp"

/* These maximums are made public so that static allocation is possible. */

#define MAXIMUM_PLATFORM_COUNT 256

#define JOYSTICK_PROFILE_XBOX 1
#define JOYSTICK_PROFILE_DUALSHOCK 2

typedef enum RepositionAlgorithm { REPOSITION_SELECT_BLINDLY, REPOSITION_SELECT_AWARELY } RepositionAlgorithm;

typedef enum RendererType { RENDERER_HARDWARE, RENDERER_SOFTWARE } RendererType;

void initialize_settings();

RepositionAlgorithm get_reposition_algorithm();

U64 get_platform_count();

int get_font_size();

int get_window_width();

int get_window_height();

U16 get_tiles_on_x();

U16 get_tiles_on_y();

int get_tile_w();

int get_tile_h();

int get_bar_height();

long get_padding();

bool get_player_stops_platforms();

int get_joystick_profile();

RendererType get_renderer_type();

int get_platform_max_width();

int get_platform_min_width();

int get_platform_max_speed();

int get_platform_min_speed();

S32 get_perk_interval();

S32 get_perk_screen_duration();

S32 get_perk_player_duration();

int is_logging_player_score();

bool should_hide_cursor();

F64 get_screen_occupancy();

#endif
