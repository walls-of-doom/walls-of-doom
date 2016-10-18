#include "clock.h"

#include "logger.h"

#include <SDL.h>

#ifdef HAS_CLOCK_GETTIME
  #include <time.h>
#endif

/**
 * Returns a number of milliseconds.
 *
 * This function should be used to measure computation times.
 */
Milliseconds get_milliseconds(void) {
  return SDL_GetTicks();
}

/**
 * Returns a number of nanoseconds.
 *
 * This function should be used to measure computation times.
 */
Nanoseconds get_nanoseconds(void) {
#ifdef HAS_CLOCK_GETTIME
  struct timespec time;
  if (clock_gettime(CLOCK_REALTIME, &time) == -1) {
    log_message("Failed to get time from CLOCK_REALTIME");
  }
  return time.tv_nsec;
#else
  /* Approximate by multiplying the get_milliseconds() result. */
  return get_milliseconds() * 1000000;
#endif
}
