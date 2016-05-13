#include "data.h"

#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * Assesses whether or not a file with the provided filename exists.
 */
int file_exists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

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

typedef enum Operation {
    READ,
    WRITE
} Operation;

void log_access(Operation operation, const size_t byte_count, const char *filename) {
    char message[512];
    if (operation == READ) {
        sprintf(message, "Reading %lu bytes from %s", (unsigned long) byte_count, filename); 
    } else {
        sprintf(message, "Writing %lu bytes to %s", (unsigned long) byte_count, filename); 
    }
    log_message(message);
}

/**
 * Writes bytes to the indicated file from the provided source.
 */
int write_bytes(const char *filename, const void *source, const size_t size, const size_t count) {
    log_access(WRITE, size * count, filename);
    FILE *file = fopen(filename, "wb");
    fwrite(source, size, count, file);
    fclose(file);
    return 0;
}

/**
 * Reads bytes from the indicated file to the provided destination.
 */
int read_bytes(const char *filename, void *destination, const size_t size, const size_t count) {
    log_access(READ, size * count, filename);
    FILE *file = fopen(filename, "rb");
    fread(destination, size, count, file);
    fclose(file);
    return 0;
}
