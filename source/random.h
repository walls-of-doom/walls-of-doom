/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * The xoroshiro128+ algorithm was written in 2016 by David Blackman and
 * Sebastiano Vigna (vigna@acm.org) and released to the public domain. We do
 * not claim ownership of their algorithm.
 *
 * See LICENSE.txt for more details.
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

/**
 * Returns the next power of two bigger than the provided number.
 */
uint64_t find_next_power_of_two(uint64_t number);

/**
 * Returns a random number in the range [minimum, maximum].
 */
int random_integer(const int minimum, const int maximum);

#endif
