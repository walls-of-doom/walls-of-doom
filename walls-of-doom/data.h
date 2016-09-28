#ifndef DATA_H
#define DATA_H

#include "code.h"

#include <linux/limits.h>

#include <stdio.h>

/**
 * The maximum size a path string may have, including the null terminator.
 */
#define MAXIMUM_PATH_SIZE PATH_MAX

/**
 * Assesses whether or not a file with the provided filename exists.
 */
int file_exists(const char *filename);

/**
 * Returns the number of lines in a file.
 *
 * This is the count of occurrences of '\n'.
 */
int file_line_count(const char *filename);

/**
 * Writes to buffer the full path for a file created by Walls of Doom.
 *
 * If one needs to access the log.txt file, one should use
 *
 *   char path[MAXIMUM_PATH_SIZE];
 *   get_full_path(path, "log.txt");
 *
 * This is the correct way to access mutable files in any platform.
 */
Code get_full_path(char *buffer, char *filename);

/**
 * Writes bytes to the indicated file from the provided source.
 *
 * Returns 0 in case of success.
 */
int write_bytes(const char *filename, const void *source, const size_t size,
                const size_t count);

/**
 * Reads bytes from the indicated file to the provided destination.
 *
 * Returns 0 in case of success.
 */
int read_bytes(const char *filename, void *destination, const size_t size,
               const size_t count);

/**
 * Reads characters from the indicated file into the provided character string.
 *
 * Returns 0 in case of success.
 */
int read_characters(const char *const filename, char *destination,
                    const size_t destination_size);

/**
 * Reads integers from the indicated file into the provided array.
 *
 * Returns the number of integers read.
 */
size_t read_integers(const char *const filename, int *integer_array,
                     const size_t integer_array_size);

#endif
