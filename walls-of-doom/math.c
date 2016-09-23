#include "math.h"

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
 * Returns the smallest of two integers.
 */
int min(const int a, const int b) {
  if (b > a) {
    return a;
  } else {
    return b;
  }
}

/**
 * Returns the biggest of two integers.
 */
int max(const int a, const int b) {
  if (b < a) {
    return a;
  } else {
    return b;
  }
}
