#include "command.h"
#include "joystick.h"
#include "player.h"
#include <SDL.h>

/**
 * Returns the Command value corresponding to the provided key combination.
 */
static Command command_from_key(const SDL_Keysym keysym) {
  const SDL_Keycode sym = keysym.sym;
  const Uint16 mod = keysym.mod;
  if (sym == SDLK_KP_8 || sym == SDLK_UP) {
    return COMMAND_UP;
  } else if (sym == SDLK_KP_4 || sym == SDLK_LEFT) {
    return COMMAND_LEFT;
  } else if (sym == SDLK_KP_5) {
    return COMMAND_CENTER;
  } else if (sym == SDLK_KP_6 || sym == SDLK_RIGHT) {
    return COMMAND_RIGHT;
  } else if (sym == SDLK_KP_2 || sym == SDLK_DOWN) {
    return COMMAND_DOWN;
  } else if (sym == SDLK_SPACE) {
    return COMMAND_JUMP;
  } else if (sym == SDLK_RETURN || sym == SDLK_KP_ENTER) {
    return COMMAND_ENTER;
  } else if (sym == SDLK_c) {
    return COMMAND_CONVERT;
  } else if (sym == SDLK_i) {
    if (mod & KMOD_SHIFT) {
      return COMMAND_INVEST_ALL;
    }
    return COMMAND_INVEST;
  } else if (sym == SDLK_p) {
    return COMMAND_PAUSE;
  } else if (sym == SDLK_q) {
    return COMMAND_QUIT;
  }
  return COMMAND_NONE;
}

/**
 * Returns the Command value corresponding to the provided input code.
 */
static Command command_from_event(const SDL_Event event) {
  if (event.type == SDL_QUIT) {
    return COMMAND_CLOSE;
  }
  if (event.type == SDL_KEYDOWN) {
    return command_from_key(event.key.keysym);
  } else if (event.type == SDL_JOYAXISMOTION) {
    return command_from_joystick_event(event);
  } else if (event.type == SDL_JOYBUTTONDOWN) {
    return command_from_joystick_event(event);
  }
  return COMMAND_NONE;
}

static int is_toggled_command(Command command) {
  return command == COMMAND_PAUSE;
}

/**
 * Returns the Command value corresponding to the provided input code.
 */
static void digest_event(const SDL_Event event, double *table) {
  Command command;
  if (event.type == SDL_QUIT) {
    table[COMMAND_CLOSE] = 1.0;
  } else if (event.type == SDL_KEYDOWN) {
    command = command_from_key(event.key.keysym);
    if (is_toggled_command(command)) {
      if (!event.key.repeat) {
        table[command] = !table[command];
      }
    } else {
      table[command] = 1.0;
    }
  } else if (event.type == SDL_KEYUP) {
    command = command_from_key(event.key.keysym);
    if (is_toggled_command(command)) {
    } else {
      table[command] = 0.0;
    }
  } else if (event.type == SDL_JOYAXISMOTION) {
    digest_joystick_event(event, table);
  } else if (event.type == SDL_JOYBUTTONDOWN) {
    digest_joystick_event(event, table);
  }
}

void initialize_command_table(CommandTable *table) {
  int i;
  for (i = 0; i < COMMAND_COUNT; i++) {
    table->table[i] = 0.0;
  }
}

void read_commands(CommandTable *table) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    digest_event(event, table->table);
  }
}

/**
 * Reads the next command that needs to be processed.
 *
 * This is the last processable pending command or COMMAND_NONE.
 */
Command read_next_command(void) {
  Command last_valid_command = COMMAND_NONE;
  Command current;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    current = command_from_event(event);
    if (current != COMMAND_NONE) {
      last_valid_command = current;
    }
  }
  return last_valid_command;
}

/**
 * Waits for the next command, blocking indefinitely.
 */
Command wait_for_next_command(void) {
  Command command = COMMAND_NONE;
  SDL_Event event;
  int got_command = 0;
  while (!got_command) {
    if (SDL_WaitEvent(&event)) {
      command = command_from_event(event);
      got_command = command != COMMAND_NONE;
    }
  }
  return command;
}

/**
 * Waits for any user input, blocking indefinitely.
 */
Code wait_for_input(void) {
  SDL_Event event;
  while (1) {
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return CODE_QUIT;
      }
      if (event.type == SDL_KEYDOWN || event.type == SDL_JOYBUTTONDOWN) {
        return CODE_OK;
      }
    } else {
      /* WaitEvent returns 0 to indicate errors. */
      return CODE_ERROR;
    }
  }
}
