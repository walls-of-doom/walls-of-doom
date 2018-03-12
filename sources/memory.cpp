#include "memory.h"
#include "logger.h"

/**
 * This module simplifies memory management and, by doing so, reduces
 * programmer mistakes.
 */

/* One gibibyte should be plenty of space for any correct call. */
#define MAXIMUM_MEMORY_SIZE 1UL << 30

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
void *resize_memory(void *pointer, size_t size) {
  void *p = NULL;
  /* Handle the special case of freeing memory. */
  if (size == 0) {
    free(pointer);
    /* Return NULL, which prevents double-freeing bugs. */
    return NULL;
  }
  if (size > MAXIMUM_MEMORY_SIZE) {
    log_message("Attempted to allocate more than the maximum memory size.");
    exit(EXIT_FAILURE);
  }
  p = realloc(pointer, size);
  if (p == NULL) {
    log_message("Failed to to resize memory.");
    exit(EXIT_FAILURE);
  }
  return p;
}
