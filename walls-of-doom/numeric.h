#ifndef NUMERIC_H
#define NUMERIC_H

/**
 * Normalizes a scalar by returning either -1, 0 or 1 if this scalar is
 * negative, zero, or positive, respectively.
 */
int normalize(const int value);

/**
 * Returns the number of digits in the provided value.
 */
int count_digits(long value);

/**
 * Returns the smallest of two integers.
 */
int min_int(const int a, const int b);

/**
 * Returns the biggest of two integers.
 */
int max_int(const int a, const int b);

#endif
