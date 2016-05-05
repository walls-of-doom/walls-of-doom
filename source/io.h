/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#ifndef IO_H
#define IO_H

#include <curses.h>

/**
 * Prints the provided string on the screen starting at (x, y).
 */
void print(const int x, const int y, const char *string) {
    mvprintw(y, x, string);
}

#endif
