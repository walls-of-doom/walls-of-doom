/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#ifndef REST_H
#define REST_H

void rest_for_ns(long ns);

/**
 * Rests for a number of seconds equal to the reciprocal of the provided
 * argument.
 */
void rest_for_second_fraction(int fps);

#endif
