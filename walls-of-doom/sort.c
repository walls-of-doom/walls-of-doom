#include "sort.h"
#include <stdlib.h>
#include <string.h>

/**
 * Sorts the first count elements of the array pointed to by start, each element
 * size bytes long.
 */
void sort(void *start, const size_t count, const size_t size,
          int (*compare)(const void *, const void *)) {
  qsort(start, count, size, compare);
}

static void swap(unsigned char *a, unsigned char *b, size_t size) {
  unsigned char a_byte;
  unsigned char b_byte;
  while (size--) {
    /*
     * Although we only need one swap byte, I think it reads better using two.
     *
     * The compiler probably optimizes this away anyway.
     */
    a_byte = *a;
    b_byte = *b;
    *a++ = b_byte;
    *b++ = a_byte;
  }
}

/**
 * Reverses the ordering of the first count elements of the array pointed to by
 * start, each element size bytes long.
 */
void reverse(void *start, const size_t count, const size_t size) {
  unsigned char *pointer = (unsigned char *)start;
  size_t i;
  for (i = 0; i < count / 2; i++) {
    swap(pointer + i * size, pointer + (count - i - 1) * size, size);
  }
}
