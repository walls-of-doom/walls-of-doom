#include "joystick.h"
#include "command.h"
#include "constants.h"
#include "logger.h"

#include <SDL.h>

#define JOYSTICK_DEAD_ZONE 4096

static SDL_Joystick *shared_joystick = NULL;

static void log_joystick_count() {
  char log_buffer[MAXIMUM_STRING_SIZE];
  sprintf(log_buffer, "Joystick count: %d", SDL_NumJoysticks());
  log_message(log_buffer);
}

SDL_Joystick *get_joystick() { return shared_joystick; }

static void set_joystick(SDL_Joystick *joystick) { shared_joystick = joystick; }

void initialize_joystick() {
  log_joystick_count();
  if (SDL_NumJoysticks()) {
    set_joystick(SDL_JoystickOpen(0));
    if (get_joystick() == NULL) {
      log_message("Failed to open controller!");
    }
  }
}

Command command_from_joystick_event(const SDL_Event event) {
  if (event.type == SDL_JOYBUTTONDOWN) {
    if (event.jbutton.button == 2) {
      return COMMAND_JUMP;
    }
  } else if (event.type == SDL_JOYAXISMOTION) {
    if (abs(event.jaxis.value) > JOYSTICK_DEAD_ZONE) {
      if (event.jaxis.axis == 0) {
        if (event.jaxis.value > 0) {
          return COMMAND_RIGHT;
        } else {
          return COMMAND_LEFT;
        }
      } else {
        if (event.jaxis.value > 0) {
          return COMMAND_DOWN;
        } else {
          return COMMAND_UP;
        }
      }
    }
  }
  return COMMAND_NONE;
}

void finalize_joystick() {
  if (get_joystick() != NULL) {
    SDL_JoystickClose(get_joystick());
    set_joystick(NULL);
  }
}
