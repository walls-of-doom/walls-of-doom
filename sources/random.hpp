#ifndef RANDOM_H
#define RANDOM_H

#include "integers.hpp"
#include <string>

/**
 * Seeds the PRNG with the current time.
 *
 * This function can safely be called multiple times.
 */
void seed_random();

/**
 * Returns the next power of two bigger than the provided number.
 */
U64 find_next_power_of_two(U64 number);

/**
 * Returns a random number in the range [minimum, maximum].
 */
int random_integer(int minimum, int maximum);

std::string get_user_name();

#endif
