#include "clock.h"

#include <SDL.h>

/**
 * Returns a number of milliseconds.
 *
 * This function should be used to measure computation times.
 */
Milliseconds get_milliseconds(void) { return SDL_GetTicks(); }
