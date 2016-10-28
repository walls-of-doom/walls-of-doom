#ifndef COMMAND_H
#define COMMAND_H

#include "code.h"

/**
 * The Command enumerated type represents the different commands the user may
 * issue.
 */
typedef enum Command {
  COMMAND_NONE,
  COMMAND_UP,
  COMMAND_LEFT,
  COMMAND_CENTER,
  COMMAND_RIGHT,
  COMMAND_DOWN,
  COMMAND_JUMP,
  COMMAND_ENTER,
  COMMAND_CONVERT,
  COMMAND_INVEST,
  COMMAND_QUIT,
  COMMAND_CLOSE
} Command;

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
