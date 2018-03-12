#include "clock.h"

#include <SDL.h>

/**
 * Returns a number of milliseconds.
 *
 * This function should be used to measure computation times.
 */
Milliseconds get_milliseconds(void) { return SDL_GetTicks(); }

/**
 * Sleeps for the specified number of milliseconds or more.
 */
void sleep_milliseconds(Milliseconds amount) {
  /* The biggest value representable by long on all platforms. */
  const Milliseconds maximum_sleep = 0x7FFFFFFFL;
  Uint32 ms;
  if (amount < 1) {
    return;
  }
  if (amount < maximum_sleep) {
    ms = (Uint32)amount;
  } else {
    /* Note that this constant is unsigned. */
    ms = 0x7FFFFFFFUL;
  }
  SDL_Delay(ms);
}
