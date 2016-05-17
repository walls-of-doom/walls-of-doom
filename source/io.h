#ifndef IO_H
#define IO_H

#include "command.h"
#include "physics.h"

typedef enum ColorScheme {
    // Color pair 0 is assumed to be white on black, but is actually whatever
    // the terminal implements before color is initialized. It cannot be
    // modified by the application, therefore we must start at 1.
    TOP_BAR_COLOR = 1, // Set the first enum constant to one
    PLATFORM_COLOR,    // Becomes two, and so on
    BOTTOM_BAR_COLOR
} ColorScheme;

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 */
void initialize(void);

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exitting.
 */
void finalize(void);

/**
 * Draws a full game to the screen.
 */
int draw(const Player * const player, const Platform *platforms, const size_t platform_count, const BoundingBox * const box);

/**
 * Prints the provided string on the screen starting at (x, y).
 */
void print(const int x, const int y, const char *string);

/**
 * Prints the provided string centered on the screen at the provided line.
 */
void print_centered(const int y, const char *string);

/**
 * Prints the provided string after formatting it to increase readability.
 */
void print_long_text(char *string);

/**
 * Prints the provided Platform, respecting the BoundingBox.
 */
void print_platform(const Platform * const platform, const BoundingBox * const box);

/**
 * Reads the next command that needs to be processed. This is the last command
 * on the input buffer.
 *
 * This function consumes the whole input buffer and returns either NO_COMMAND
 * (if no other Command could be produced by what was in the input buffer) or
 * the last Command different than NO_COMMAND that could be produced by what
 * was in the input buffer.
 */
Command read_next_command(void);

/**
 * Waits for user input, indefinitely.
 */
Command wait_for_next_command(void);

#endif
