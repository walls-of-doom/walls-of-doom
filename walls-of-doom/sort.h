#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

/**
 * Sorts the first count elements of the array pointed to by start, each element
 * size bytes long.
 */
void sort(void *start, const size_t count, const size_t size, int (*compare)(const void *, const void *));

/**
 * Reverses the ordering of the first count elements of the array pointed to by
 * start, each element size bytes long.
 */
void reverse(void *start, const size_t count, const size_t size);

#endif
