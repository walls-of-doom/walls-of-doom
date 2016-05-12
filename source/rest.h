#ifndef REST_H
#define REST_H

#include <time.h>
#include <stdint.h>

#define NANOSECONDS_IN_ONE_SECOND      1000000000UL
#define NANOSECONDS_IN_ONE_MILLISECOND 1000000UL
#define NANOSECONDS_IN_ONE_MICROSECOND 1000UL

/**
 * Returns the nanoseconds difference between the two provided timespecs.
 */
uint_least64_t elapsed_time_in_nanoseconds(const struct timespec * const start, const struct timespec * const end);

/**
 * Rests for the specified number of nanoseconds.
 */
void rest_for_nanoseconds(uint_least64_t nanoseconds);

/**
 * Rests for the reciprocal of the provided argument seconds.
 */
void rest_for_second_fraction(int fps);

#endif
