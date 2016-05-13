#ifndef DATA_H
#define DATA_H

#include <stdio.h>

/**
 * Assesses whether or not a file with the provided filename exists.
 */
int file_exists(const char* filename);

/**
 * Reads integers from the indicated file into the provided array.
 *
 * The array should be big enough for the provided file.
 *
 * Returns the number of integers read.
 */
size_t read_integers(const char * const filename, int *integer_array);

/**
 * Writes bytes to the indicated file from the provided source.
 */
int write_bytes(const char *filename, const void *source, const size_t size, const size_t count);

/**
 * Reads bytes from the indicated file to the provided destination.
 */
int read_bytes(const char *filename, void *destination, const size_t size, const size_t count);

#endif
