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
    mvprintw(y, x, string);
}

void print_platform(const Platform * const platform) {
    unsigned int i;
    for (i = 0; i < platform->width; i++) {
        print(platform->x + i, platform->y, "█");
    }
}

#endif
