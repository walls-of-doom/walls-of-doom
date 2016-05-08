#ifndef COMMAND_H
#define COMMAND_H

/**
 * The Command enumerated type represents the different commands the user may issue.
 */
typedef enum Command {
    NO_COMMAND,
    COMMAND_UP,
    COMMAND_LEFT,
    COMMAND_CENTER,
    COMMAND_RIGHT,
    COMMAND_DOWN,
    COMMAND_JUMP,
    COMMAND_ENTER,
    COMMAND_QUIT
} Command;

#endif
