#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

/* The maximum number of bytes insertion_sort() can handle. */
/* If you change this constant, remember to update the function documentation. */
#define INSERTION_SORT_MAXIMUM_SIZE 1024

/**
 * Insertion sort for a generic contiguous chunk of memory that can be compared by a function.
 *
 * This function only works for elements up to 1024 bytes (1 KiB) in size.
 *
 * start - a pointer of the first address to be in the sorted range
 * count - how many elements there are to sort
 * width - the width (in bytes) of each element, should be in the range [1, 1024]
 * compare - a function that returns a negative integer if the first argument
 *           is less than the second, 0 if they are equal, or a positive
 *           integer if the first argument is greater than the second one.
 */
void insertion_sort(void *start, size_t count, size_t width, int (*compare)(const void*, const void*));

/**
 * Reverses the ordering of a generic contiguous chunk of memory.
 */
void reverse(void *start, size_t count, size_t width);

#endif
