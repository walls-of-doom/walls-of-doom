#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "command.h"
#include "player.h"
#include <SDL.h>

void initialize_joystick(void);

SDL_Joystick *get_joystick(void);

Command command_from_joystick_event(const SDL_Event event);

void digest_joystick_event(const SDL_Event event, double *table);

void finalize_joystick(void);

#endif
