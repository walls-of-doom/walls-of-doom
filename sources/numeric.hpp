#ifndef NUMERIC_H
#define NUMERIC_H

#include "integers.hpp"
#include <limits>
#include <stdexcept>

template <typename T, typename K> void safe_add(T &accumulator, K value) {
  static_assert(std::is_integral<T>::value, "T must be integral.");
  static_assert(std::is_integral<K>::value, "K must be integral.");
  static_assert(std::is_unsigned<T>::value, "T must be unsigned.");
  if (value == 0) {
    return;
  }
  if (value < 0) {
    if (std::abs(value) > std::abs(accumulator)) {
      throw std::logic_error("Underflow.");
    }
  }
  if (value > 0) {
    if (std::abs(value) > std::numeric_limits<T>::max() - accumulator) {
      throw std::logic_error("Overflow.");
    }
  }
  accumulator += value;
}

/**
 * Normalizes a scalar by returning either -1, 0 or 1 if this scalar is negative, zero, or positive, respectively.
 */
inline S32 normalize(const S32 value) {
  if (value < 0) {
    return -1;
  }
  if (value == 0) {
    return 0;
  }
  return 1;
}

/**
 * Returns the number of digits in the provided value.
 */
inline S32 count_digits(long value) {
  S32 digits = 1;
  value = labs(value);
  while (value >= 10) {
    digits++;
    value /= 10;
  }
  return digits;
}

#endif
