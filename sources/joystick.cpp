#include "joystick.hpp"
#include "command.hpp"
#include "constants.hpp"
#include "logger.hpp"
#include "settings.hpp"

#include <SDL.h>

#define XBOX_A 0
#define XBOX_B 1
#define XBOX_X 2
#define XBOX_Y 3
#define XBOX_BACK 4
#define XBOX_GUIDE 5
#define XBOX_START 6
#define XBOX_LEFTSTICK 7
#define XBOX_RIGHTSTICK 8
#define XBOX_LEFTSHOULDER 9
#define XBOX_RIGHTSHOULDER 10
#define XBOX_DPAD_UP 11
#define XBOX_DPAD_DOWN 12
#define XBOX_DPAD_LEFT 13
#define XBOX_DPAD_RIGHT 14

/* DualShock constants. L3 and R3 are the analog sticks pressed down. */
#define DUALSHOCK_TRIANGLE 0
#define DUALSHOCK_CIRCLE 1
#define DUALSHOCK_CROSS 2
#define DUALSHOCK_SQUARE 3
#define DUALSHOCK_L1 4
#define DUALSHOCK_L2 6
#define DUALSHOCK_L3 10
#define DUALSHOCK_R1 5
#define DUALSHOCK_R2 7
#define DUALSHOCK_R3 11
#define DUALSHOCK_SELECT 8
#define DUALSHOCK_START 9

static SDL_Joystick *shared_joystick = nullptr;

static void log_joystick_count() {
  char log_buffer[MAXIMUM_STRING_SIZE];
  sprintf(log_buffer, "Joystick count is %d.", SDL_NumJoysticks());
  log_message(log_buffer);
}

SDL_Joystick *get_joystick() {
  return shared_joystick;
}

static void set_joystick(SDL_Joystick *joystick) {
  shared_joystick = joystick;
}

void initialize_joystick() {
  log_joystick_count();
  if (SDL_NumJoysticks() != 0) {
    set_joystick(SDL_JoystickOpen(0));
    if (get_joystick() == nullptr) {
      log_message("Failed to open controller!");
    }
  }
}

static int get_convert_button(const Settings &settings) {
  if (settings.get_joystick_profile() == JOYSTICK_PROFILE_XBOX) {
    return XBOX_B;
  }
  return DUALSHOCK_CIRCLE;
}

static int get_jump_button(const Settings &settings) {
  if (settings.get_joystick_profile() == JOYSTICK_PROFILE_XBOX) {
    return XBOX_A;
  }
  return DUALSHOCK_CROSS;
}

static int get_enter_button(const Settings &settings) {
  if (settings.get_joystick_profile() == JOYSTICK_PROFILE_XBOX) {
    return XBOX_START;
  }
  return DUALSHOCK_START;
}

static int get_pause_button(const Settings &settings) {
  if (settings.get_joystick_profile() == JOYSTICK_PROFILE_XBOX) {
    return XBOX_BACK;
  }
  return DUALSHOCK_SELECT;
}

Command command_from_joystick_button(const Settings &settings, Uint8 button) {
  if (button == get_convert_button(settings)) {
    return COMMAND_CONVERT;
  }
  if (button == get_jump_button(settings)) {
    return COMMAND_JUMP;
  }
  if (button == get_enter_button(settings)) {
    return COMMAND_ENTER;
  }
  if (button == get_pause_button(settings)) {
    return COMMAND_PAUSE;
  }
  return COMMAND_NONE;
}

Command command_from_joystick_event(const Settings &settings, const SDL_Event event) {
  if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP) {
    return command_from_joystick_button(settings, event.jbutton.button);
  }
  if (event.type == SDL_JOYAXISMOTION) {
    if (abs(event.jaxis.value) > JOYSTICK_DEAD_ZONE) {
      if (event.jaxis.axis == 0) {
        if (event.jaxis.value > 0) {
          return COMMAND_RIGHT;
        }
        if (event.jaxis.value < 0) {
          return COMMAND_LEFT;
        }
      } else {
        if (event.jaxis.value > 0) {
          return COMMAND_DOWN;
        }
        if (event.jaxis.value < 0) {
          return COMMAND_UP;
        }
      }
    }
  }
  return COMMAND_NONE;
}

void finalize_joystick() {
  if (get_joystick() != nullptr) {
    SDL_JoystickClose(get_joystick());
    set_joystick(nullptr);
  }
}
