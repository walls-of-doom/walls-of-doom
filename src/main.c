/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "insertion-sort.h"
#include "array-utils.h"

int compare_pointers_to_longs(const void *pointer_to_a, const void *pointer_to_b) {
    // This one line solution is not safe
    //   return *(long *)(pointer_to_a) - *(long *)(pointer_to_b);
    // as it may overflow and underflow.
    // Instead, we use my other one-liner, which unfortunately requires branching
    return *(long *)(pointer_to_a) < *(long *)(pointer_to_b) ? -1 : *(long *)(pointer_to_a) == *(long *)(pointer_to_b) ? 0 : 1;
}

char *pointer_to_long_to_string(const void *pointer_to_long) {
    char *array_start = malloc(256); // Good enough for most long implementations.
    sprintf(array_start, "%ld", *(long *)(pointer_to_long));
    return array_start;
}

int main() {
    long longs[] = {-6, 8, 2, 0, 0, -9, -8, 0, 5, 6, 9};
    write_array_to_console(longs, sizeof(longs) / sizeof(long), sizeof(long), pointer_to_long_to_string);
    insertion_sort(longs, sizeof(longs) / sizeof(long), sizeof(long), compare_pointers_to_longs);
    write_array_to_console(longs, sizeof(longs) / sizeof(long), sizeof(long), pointer_to_long_to_string);
}
