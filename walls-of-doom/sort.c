#include "sort.h"

#include <stdlib.h>
#include <string.h>

/**
 * Insertion sort for a generic contiguous chunk of memory that can be compared
 * by a function.
 *
 * This function only works for elements up to 1024 bytes (1 KiB) in size.
 *
 * start - a pointer of the first address to be in the sorted range
 * count - how many elements there are to sort
 * width - the width (in bytes) of each element, should be in the range [1,
 * 1024]
 * compare - a function that returns a negative integer if the first argument
 *           is less than the second, 0 if they are equal, or a positive
 *           integer if the first argument is greater than the second one.
 */
void insertion_sort(void *start, size_t count, size_t width,
                    int (*compare)(const void *, const void *)) {
  unsigned char helper[INSERTION_SORT_MAXIMUM_SIZE];
  unsigned char *pointer = (unsigned char *)start;
  size_t i;
  size_t j;
  void *pointer_to_element;
  void *pointer_to_predecessor;
  for (i = 0; i + 1 < count; i++) {
    j = i + 1; /* j is always positive here */
    pointer_to_element = (void *)(pointer + j * width);
    pointer_to_predecessor = (void *)(pointer + (j - 1) * width);
    /* While the element is not the first one and is smaller than the
     * predecessor. */
    while (j > 0 &&
           (*compare)(pointer_to_element, pointer_to_predecessor) < 0) {
      /* Swap */
      memcpy(helper, pointer_to_predecessor, width);
      memcpy(pointer_to_predecessor, pointer_to_element, width);
      memcpy(pointer_to_element, helper, width);
      /* Update the pointers */
      pointer_to_element = pointer_to_predecessor;
      /*
       * Avoid void pointer arithmetic by casting the pointer to a byte
       * pointer before subtracting and casting it back to void pointer
       * afterwards.
       */
      pointer_to_predecessor =
          (void *)((unsigned char *)(pointer_to_predecessor)-width);
      /* Update the index */
      j--;
    }
  }
}

/**
 * Reverses the ordering of a generic contiguous chunk of memory.
 *
 * This function only works for elements up to 1024 bytes (1 KiB) in size.
 */
void reverse(void *start, size_t count, size_t width) {
  unsigned char helper[INSERTION_SORT_MAXIMUM_SIZE];
  unsigned char *pointer = (unsigned char *)start;
  size_t i;
  for (i = 0; i < count / 2; i++) {
    size_t j = count - i - 1;
    /* Swap i and j */
    void *pointer_to_i = (void *)(pointer + i * width);
    void *pointer_to_j = (void *)(pointer + j * width);
    memcpy(helper, pointer_to_i, width);
    memcpy(pointer_to_i, pointer_to_j, width);
    memcpy(pointer_to_j, helper, width);
  }
}
