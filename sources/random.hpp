#ifndef RANDOM_H
#define RANDOM_H

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
unsigned long find_next_power_of_two(unsigned long number);

/**
 * Returns a random number in the range [minimum, maximum].
 */
int random_integer(int minimum, int maximum);

std::string get_user_name();

#endif
