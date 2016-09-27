#ifndef IO_H
#define IO_H

#include "code.h"
#include "color.h"
#include "command.h"
#include "game.h"
#include "perk.h"
#include "physics.h"

void present(SDL_Renderer *renderer);

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 *
 * Returns 0 in case of success.
 */
int initialize(SDL_Window **window, SDL_Renderer **renderer);

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exiting.
 *
 * Returns 0 in case of success.
 */
int finalize(SDL_Window **window, SDL_Renderer **renderer);

/**
 * Reads a string from the user of up to size characters (including NUL).
 *
 * The string will be echoed after the prompt, which starts at (x, y).
 *
 * Returns 0 if successful.
 * Returns 1 if the user tried to quit.
 */
int read_string(const int x, const int y, const char *prompt, char *destination,
                size_t size, SDL_Renderer *renderer);

void read_player_name(char *destination, const size_t maximum_size,
                      SDL_Renderer *renderer);

/**
 * Draws a full game to the screen.
 */
int draw_game(const Game *const game, SDL_Renderer *renderer);

/**
 * Prints the provided string on the screen starting at (x, y).
 *
 * Returns 0 in case of success.
 */
int print(const int x, const int y, const char *string, ColorPair color_pair,
          SDL_Renderer *renderer);

/**
 * Prints the provided string centered on the screen at the provided line.
 */
void print_centered(const int y, const char *string, ColorPair color_pair,
                    SDL_Renderer *renderer);

/**
 * Prints the provided string after formatting it to increase readability.
 */
void print_long_text(char *string, SDL_Renderer *renderer);

/**
 * Prints the provided Platform, respecting the BoundingBox.
 */
void print_platform(const Platform *const platform,
                    const BoundingBox *const box, SDL_Renderer *renderer);

void print_game_result(const char *name, const unsigned int score,
                       const int position, SDL_Renderer *renderer);

ColorPair get_perk_color(Perk perk);

/**
 * Returns a BoundingBox that represents the playable area after removing bars
 * and margins.
 */
BoundingBox bounding_box_from_screen(void);

/**
 * Reads the next command that needs to be processed.
 *
 * This is the last pending command.
 *
 * This function consumes the whole input buffer and returns either
 * COMMAND_NONE (if no other Command could be produced by what was in the input
 * buffer) or the last Command different than COMMAND_NONE that could be
 * produced by what was in the input buffer.
 */
Command read_next_command(void);

/**
 * Waits for the next command, blocking indefinitely.
 */
Command wait_for_next_command(void);

/**
 * Waits for any user input, blocking indefinitely.
 */
Code wait_for_input(void);

#endif
