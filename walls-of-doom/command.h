#ifndef COMMAND_H
#define COMMAND_H

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
  COMMAND_QUIT,
  COMMAND_CLOSE
} Command;

#endif
