#ifndef BASE_IO_H
#define BASE_IO_H

#include "clock.hpp"
#include "code.hpp"
#include "color.hpp"
#include "command.hpp"
#include "game.hpp"
#include "perk.hpp"
#include "physics.hpp"
#include "record.hpp"
#include <SDL_ttf.h>
#include <string>
#include <vector>

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

Font *get_font();

int get_font_width();

int get_font_height();

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

Code print_absolute(const int x, const int y, const char *string, const ColorPair color_pair, Renderer *renderer);

/**
 * Prints the provided strings centered at the specified absolute line.
 */
Code print_centered_horizontally(const int y, const std::vector<std::string> &strings, const ColorPair color_pair,
                                 Renderer *renderer);

Code print_centered_vertically(const std::vector<std::string> &strings, const ColorPair color_pair, Renderer *renderer);

#endif
