#ifndef ABOUT_H
#define ABOUT_H

#include "code.h"
#include "command.h"
#include <SDL.h>

/**
 * Presents information about the game to the player.
 */
Code info(SDL_Renderer *renderer, CommandTable *table);

#endif
