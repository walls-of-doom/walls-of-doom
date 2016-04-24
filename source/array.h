/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdio.h>

/**
 * Write an array of elements to the console.
 *
 * start - a pointer of the first address
 * count - how many elements there are
 * width - the width (in bytes) of each element
 * convert - a function that converts an element to a string
 */
void write_array_to_console(void *start, size_t count, size_t width, char *(*to_string)(const void*)) {
    printf("{ ");
    unsigned char *pointer = (unsigned char *)start;
    size_t i = 0;
    for (i = 0; i < count; i++) {
        char *string = (*to_string)((void *)(pointer + i * width));
        if (i > 0) {
            printf("  ");
        }
        printf("%s", string);
        if (i < count - 1) {
            printf(",\n");
        }
        // Free the memory used by the string.
        free(string);
    }
    printf(" }\n");
}

#endif
