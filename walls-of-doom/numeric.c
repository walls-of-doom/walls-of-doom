#include "numeric.h"
#include <stdlib.h>

/**
 * Normalizes a scalar by returning either -1, 0 or 1 if this scalar is
 * negative, zero, or positive, respectively.
 */
int normalize(const int value) {
  if (value < 0) {
    return -1;
  } else if (value == 0) {
    return 0;
  } else {
    return 1;
  }
}

/**
 * Returns the number of digits in the provided value.
 */
int count_digits(long value) {
  /* Could use log(), which is ISO 90, but this seems cleaner. */
  int digits = 1;
  value = labs(value);
  while (value >= 10) {
    digits++;
    value /= 10;
  }
  return digits;
}

/**
 * Returns the smallest of two integers.
 */
int min_int(const int a, const int b) {
  if (b > a) {
    return a;
  } else {
    return b;
  }
}

/**
 * Returns the biggest of two integers.
 */
int max_int(const int a, const int b) {
  if (b < a) {
    return a;
  } else {
    return b;
  }
}
