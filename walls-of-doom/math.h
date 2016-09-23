#ifndef MATH_H
#define MATH_H

/**
 * Normalizes a scalar by returning either -1, 0 or 1 if this scalar is
 * negative, zero, or positive, respectively.
 */
int normalize(const int value);

/**
 * Returns the smallest of two integers.
 */
int min(const int a, const int b);

/**
 * Returns the biggest of two integers.
 */
int max(const int a, const int b);

#endif
