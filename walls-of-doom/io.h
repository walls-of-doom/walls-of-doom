#ifndef IO_H
#define IO_H

#include "clock.h"
#include "code.h"
#include "color.h"
#include "command.h"
#include "game.h"
#include "perk.h"
#include "physics.h"
#include "record.h"

/**
 * Clears the screen.
 */
void clear(SDL_Renderer *renderer);

/**
 * Updates the screen with what has been rendered.
 */
void present(SDL_Renderer *renderer);

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 */
Code initialize(SDL_Window **window, SDL_Renderer **renderer);

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exiting.
 */
Code finalize(SDL_Window **window, SDL_Renderer **renderer);

/**
 * Reads a string from the user of up to size characters (including NUL).
 *
 * The string will be echoed after the prompt, which starts at (x, y).
 */
Code read_string(const int x, const int y, const char *prompt,
                 char *destination, const size_t size, SDL_Renderer *renderer);

/**
 * Attempts to read a player name.
 *
 * Returns a Code, which may indicate that the player tried to quit.
 */
Code read_player_name(char *destination, const size_t maximum_size,
                      SDL_Renderer *renderer);

/**
 * Draws a full game to the screen.
 *
 * Returns a Milliseconds approximation of the time this function took.
 */
Milliseconds draw_game(const Game *const game, SDL_Renderer *renderer);

/**
 * Prints the provided string on the screen starting at (x, y).
 */
Code print(const int x, const int y, const char *string,
           const ColorPair color_pair, SDL_Renderer *renderer);

/**
 * Prints the provided strings centered at the specified absolute line.
 */
Code print_centered_horizontally(const int y, const int string_count,
                                 const char *const *strings,
                                 const ColorPair color_pair,
                                 SDL_Renderer *renderer);

/**
 * Prints the provided strings centered in the middle of the screen.
 */
Code print_centered_vertically(const int string_count,
                               const char *const *strings,
                               const ColorPair color_pair,
                               SDL_Renderer *renderer);

/**
 * Writes the menu, centralized in the string.
 */
void print_menu(const int line_count, char const *const *const lines,
                SDL_Renderer *renderer);

/**
 * Prints the provided string after formatting it to increase readability.
 */
void print_long_text(char *string, SDL_Renderer *renderer);

void print_game_result(const char *name, const unsigned int score,
                       const int position, SDL_Renderer *renderer);

void print_records(const size_t count, const Record *records,
                   SDL_Renderer *renderer);

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
