#ifndef MEMORY_H
#define MEMORY_H

#include <cstdlib>

/**
 * This module aims to simplify memory management.
 *
 * Besides reducing human mistakes, this also allows the program to log and
 * profile dynamic memory manipulation, and prevent double-freeing bugs.
 */

/**
 * Resizes the memory space pointed to by the pointer to size bytes.
 *
 * To allocate new memory, the pointer should be NULL.
 * To free memory, size should be zero.
 *
 * This function returns NULL if, and only if, size is zero.
 *
 * When using this function to free memory, the following syntax is recommended
 *
 *   pointer = resize_memory(pointer, 0);
 *
 * As it prevents double-freeing bugs.
 */
void *resize_memory(void *pointer, size_t size);

#endif
