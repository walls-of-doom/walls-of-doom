#include "command.h"
#include <SDL.h>

/**
 * Returns the Command value corresponding to the provided input code.
 */
static Command command_from_event(const SDL_Event event) {
  SDL_Keycode keycode;
  if (event.type == SDL_QUIT) {
    return COMMAND_CLOSE;
  }
  if (event.type == SDL_KEYDOWN) {
    keycode = event.key.keysym.sym;
    if (keycode == SDLK_KP_8 || keycode == SDLK_UP) {
      return COMMAND_UP;
    } else if (keycode == SDLK_KP_4 || keycode == SDLK_LEFT) {
      return COMMAND_LEFT;
    } else if (keycode == SDLK_KP_5) {
      return COMMAND_CENTER;
    } else if (keycode == SDLK_KP_6 || keycode == SDLK_RIGHT) {
      return COMMAND_RIGHT;
    } else if (keycode == SDLK_KP_2 || keycode == SDLK_DOWN) {
      return COMMAND_DOWN;
    } else if (keycode == SDLK_SPACE) {
      return COMMAND_JUMP;
    } else if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER) {
      return COMMAND_ENTER;
    } else if (keycode == SDLK_c) {
      return COMMAND_CONVERT;
    } else if (keycode == SDLK_q) {
      return COMMAND_QUIT;
    }
  }
  return COMMAND_NONE;
}

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
  Command command;
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
      if (event.type == SDL_KEYDOWN) {
        return CODE_OK;
      }
    } else {
      /* WaitEvent returns 0 to indicate errors. */
      return CODE_ERROR;
    }
  }
}
