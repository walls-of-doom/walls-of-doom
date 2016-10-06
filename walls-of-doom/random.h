#ifndef RANDOM_H
#define RANDOM_H

/**
 * Seeds the PRNG with the current time.
 *
 * This function can safely be called multiple times.
 */
void seed_random(void);

/**
 * Returns the next power of two bigger than the provided number.
 */
unsigned long find_next_power_of_two(const unsigned long number);

/**
 * Returns a random number in the range [minimum, maximum].
 */
int random_integer(const int minimum, const int maximum);

/**
 * Writes a pseudorandom name to the destination.
 *
 * The destination should have at least 2 * MAXIMUM_WORD_SIZE bytes.
 */
void random_name(char *destination);

#endif
