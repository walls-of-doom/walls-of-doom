#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

typedef uint32_t Milliseconds;

/**
 * Returns a number of milliseconds.
 *
 * This function should be used to measure computation times.
 */
Milliseconds get_milliseconds(void);

#endif
