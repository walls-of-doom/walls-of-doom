#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "command.h"
#include "player.h"
#include <SDL.h>

#define JOYSTICK_DEAD_ZONE 4096
#define MAXIMUM_JOYSTICK_AXIS_VALUE 32768

void initialize_joystick(void);

SDL_Joystick *get_joystick(void);

Command command_from_joystick_event(const SDL_Event event);

void finalize_joystick(void);

#endif
