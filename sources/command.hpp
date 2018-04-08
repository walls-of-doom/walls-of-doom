#ifndef COMMAND_H
#define COMMAND_H

#include "clock.hpp"
#include "code.hpp"
#include "settings.hpp"

/**
 * The Command enumeration represents the different commands the user may issue.
 */
enum Command {
  COMMAND_NONE,
  COMMAND_UP,
  COMMAND_LEFT,
  COMMAND_CENTER,
  COMMAND_RIGHT,
  COMMAND_DOWN,
  COMMAND_JUMP,
  COMMAND_ENTER,
  COMMAND_CONVERT,
  COMMAND_PAUSE,
  COMMAND_DEBUG,
  COMMAND_QUIT,
  COMMAND_CLOSE,
  COMMAND_COUNT
};

class CommandTable {
public:
  double status[COMMAND_COUNT];
  Milliseconds last_issued[COMMAND_COUNT];
  Milliseconds last_modified[COMMAND_COUNT];
};

void initialize_command_table(CommandTable *table);

bool test_command_table(CommandTable *table, enum Command command, Milliseconds repetition_delay);

void read_commands(const Settings &settings, CommandTable *table);

/**
 * Waits for any user input, blocking indefinitely.
 */
Code wait_for_input(const Settings &settings, CommandTable *table);

#endif
