#ifndef BASE_IO_H
#define BASE_IO_H

#include "clock.h"
#include "code.h"
#include "color.h"
#include "command.h"
#include "game.h"
#include "perk.h"
#include "physics.h"
#include "record.h"
#include <SDL_ttf.h>

typedef TTF_Font Font;
typedef SDL_Window Window;
typedef SDL_Renderer Renderer;

/**
 * Clears the screen.
 */
void clear(Renderer *renderer);

/**
 * Updates the screen with what has been rendered.
 */
void present(Renderer *renderer);

int get_window_width(void);

int get_window_height(void);

/**
 * Returns the height of the top and bottom bars.
 */
int get_bar_height(void);

Font *get_font(void);

int get_font_width(void);

int get_font_height(void);

int get_tile_width(void);

int get_tile_height(void);

int get_border_width(void);

int get_border_height(void);

/**
 * Returns the tile width on the screen.
 */
int get_tile_width(void);

/**
 * Returns the tile height on the screen.
 */
int get_tile_height(void);

/**
 * Swap the renderer color by the provided color.
 *
 * The color previously in the renderer will be copied to the pointer.
 */
void swap_color(Renderer *renderer, SDL_Color *color);

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 */
Code initialize(Window **window, Renderer **renderer);

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exiting.
 */
Code finalize(Window **window, Renderer **renderer);

Code print_absolute(const int x, const int y, const char *string,
                    const ColorPair color_pair, Renderer *renderer);

/**
 * Prints the provided strings centered at the specified absolute line.
 */
Code print_centered_horizontally(const int y, const int string_count,
                                 char **strings, const ColorPair color_pair,
                                 Renderer *renderer);

/**
 * Prints the provided strings centered in the middle of the screen.
 */
Code print_centered_vertically(const int string_count, char **strings,
                               const ColorPair color_pair, Renderer *renderer);

#endif
