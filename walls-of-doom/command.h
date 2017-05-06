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
  COMMAND_INVEST_ALL,
  COMMAND_PAUSE,
  COMMAND_QUIT,
  COMMAND_CLOSE,
  COMMAND_COUNT
} Command;

/**
 * Reads the next command that needs to be processed.
 *
 * This is the last processable pending command or COMMAND_NONE.
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
