#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

/**
 * Seeds the PRNG with the current time.
 *
 * This function can safely be called multiple times.
 */
void seed_random();

/**
 * Returns the next power of two bigger than the provided number.
 */
uint64_t find_next_power_of_two(uint64_t number);

/**
 * Returns a random number in the range [minimum, maximum].
 */
int random_integer(const int minimum, const int maximum);

#endif
