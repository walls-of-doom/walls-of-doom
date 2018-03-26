#include "sort.hpp"

/**
 * Sorts the first count elements of the array pointed to by start, each element
 * size bytes long.
 */
void sort(void *start, const size_t count, const size_t size, int (*compare)(const void *, const void *)) {
  qsort(start, count, size, compare);
}

static void swap(unsigned char *a, unsigned char *b, size_t size) {
  while ((size--) != 0u) {
    const auto a_byte = *a;
    const auto b_byte = *b;
    *a++ = b_byte;
    *b++ = a_byte;
  }
}

/**
 * Reverses the ordering of the first count elements of the array pointed to by
 * start, each element size bytes long.
 */
void reverse(void *start, const size_t count, const size_t size) {
  auto *pointer = static_cast<unsigned char *>(start);
  size_t i;
  for (i = 0; i < count / 2; i++) {
    swap(pointer + i * size, pointer + (count - i - 1) * size, size);
  }
}
