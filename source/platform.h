/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

typedef struct Platform {
    int x;
    int y;
    unsigned int width;
    int speed;
} Platform;

Platform *update(Platform * const platform) {
    platform->x += platform->speed;
    return platform;
}

#endif
