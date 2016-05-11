#ifndef IO_H
#define IO_H

#include "command.h"
#include "physics.h"

/**
 * Prints the provided string on the screen starting at (x, y).
 */
void print(const int x, const int y, const char *string);

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
