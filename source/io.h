/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#ifndef IO_H
#define IO_H

#include <curses.h>

#include "platform.h"

/**
 * Prints the provided string on the screen starting at (x, y).
 */
void print(const int x, const int y, const char *string) {
    // Validate that x and y are nonnegative.
    if (x < 0 || y < 0) {
        return;
    }
    mvprintw(y, x, string);
}

/**
 * Prints the provided Platform, respecting the BoundingBox.
 */
void print_platform(const Platform * const platform, const BoundingBox * const box) {
    int i;
    for (i = 0; i < platform->width; i++) {
        const int x = platform->x + i;
        const int y = platform->y;
        if (x >= box->min_x && x <= box->max_x && y >= box->min_y && y <= box->max_y) {
            print(x, y, " ");
        }
    }
}

#endif
