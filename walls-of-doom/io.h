#ifndef IO_H
#define IO_H

#include "codes.h"
#include "command.h"
#include "game.h"
#include "perks.h"
#include "physics.h"

typedef enum ColorScheme {
  /* Color pair 0 is assumed to be white on black, but is actually whatever */
  /* the terminal implements before color is initialized. It cannot be */
  /* modified by the application, therefore we must start at 1. */
  COLOR_TOP_BAR = 1, /* Set the first enum constant to one */
  COLOR_BOTTOM_BAR,  /* Becomes two, and so on */
  COLOR_PLATFORMS,
  COLOR_PLAYER,
  COLOR_INVINCIBILITY,
  COLOR_LEVITATION,
  COLOR_LOW_GRAVITY,
  COLOR_SUPER_JUMP,
  COLOR_TIME_STOP,
  COLOR_EXTRA_LIFE,
  COLOR_EXTRA_POINTS
} ColorScheme;

void clean(SDL_Renderer *renderer);

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
 * Returns a pointer to the start of the text of the string.
 *
 * This is either the first character which is not a space or '\0'.
 */
char *find_start_of_text(char *string);

/**
 * Returns a pointer to the end of the text of the string.
 *
 * This is either the first trailing space or '\0'.
 */
char *find_end_of_text(char *string);

/**
 * Trims a string by removing whitespace from its start and from its end.
 */
void trim_string(char *string);

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
int print(const int x, const int y, const char *string, SDL_Renderer *renderer);

/**
 * Prints the provided string centered on the screen at the provided line.
 */
void print_centered(const int y, const char *string, SDL_Renderer *renderer);

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

ColorScheme get_perk_color(Perk perk);

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
