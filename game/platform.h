#ifndef PLATFORM_H
#define PLATFORM_H

typedef struct Platform {
    int x;
    int y;
    int speed_x;
    int speed_y;
    int width;
} Platform;

/**
 * Reads platforms from the text file into the provided buffer.
 *
 * Returning the number of platforms read.
 *
 * This number is guaranteed to be less than MAXIMUM_PLATFORM_COUNT.
 */
int read_platforms(Platform *platforms);

#endif
