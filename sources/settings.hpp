#ifndef SETTINGS_H
#define SETTINGS_H

#include "investment.hpp"

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

int get_tile_width();

int get_tile_height();

int get_bar_height();

long get_padding();

bool get_player_stops_platforms();

int get_joystick_profile();

InvestmentMode get_investment_mode();

int get_investment_amount();

double get_investment_proportion();

int get_investment_period();

double get_investment_maximum_factor();

double get_investment_minimum_factor();

RendererType get_renderer_type();

int get_platform_max_width();

int get_platform_min_width();

int get_platform_max_speed();

int get_platform_min_speed();

int is_logging_player_score();

#endif
