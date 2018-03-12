#ifndef SETTINGS_H
#define SETTINGS_H

#include "investment.h"

/* These maximums are made public so that static allocation is possible. */

#define MAXIMUM_PLATFORM_COUNT 256

#define JOYSTICK_PROFILE_XBOX 1
#define JOYSTICK_PROFILE_DUALSHOCK 2

typedef enum RepositionAlgorithm { REPOSITION_SELECT_BLINDLY, REPOSITION_SELECT_AWARELY } RepositionAlgorithm;

typedef enum RendererType { RENDERER_HARDWARE, RENDERER_SOFTWARE } RendererType;

void initialize_settings(void);

RepositionAlgorithm get_reposition_algorithm(void);

long get_platform_count(void);

int get_font_size(void);

int get_window_width(void);

int get_window_height(void);

int get_tile_width(void);

int get_tile_height(void);

int get_bar_height(void);

long get_padding(void);

int get_player_stops_platforms(void);

int get_joystick_profile(void);

InvestmentMode get_investment_mode(void);

int get_investment_amount(void);

double get_investment_proportion(void);

int get_investment_period(void);

double get_investment_maximum_factor(void);

double get_investment_minimum_factor(void);

RendererType get_renderer_type(void);

int get_platform_max_width(void);

int get_platform_min_width(void);

int get_platform_max_speed(void);

int get_platform_min_speed(void);

int is_logging_player_score(void);

#endif
