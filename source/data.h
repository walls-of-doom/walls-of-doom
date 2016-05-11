#ifndef DATA_H
#define DATA_H

#include <stdio.h>

/**
 * Reads integers from the indicated file into the provided array.
 *
 * The array should be big enough for the provided file.
 *
 * Returns the number of integers read.
 */
size_t read_integers(const char * const filename, int *integer_array) {
    FILE *file;
    file = fopen(filename, "r");
    size_t next_index = 0;
    while (fscanf(file, "%d", &integer_array[next_index]) != EOF) {
        next_index++;
    }
    fclose(file);
    return next_index; // number of elements == index of next element
}

/**
 * Write bytes to the indicated file.
 */
int write_bytes(const char * const filename, const void * const array, const size_t count, const size_t size) {
    FILE *file;
    file = fopen(filename, "wb");
    fwrite(array, size, count, file);
    fclose(file);
    return 0;
}

#endif
