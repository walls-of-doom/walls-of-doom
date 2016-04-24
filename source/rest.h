/**
 * Copyright (c) 2016, Bernardo Sulzbach and Leonardo Ferrazza
 * All rights reserved.
 *
 * See LICENSE.txt for more details.
 */

#ifndef REST_H
#define REST_H

#ifdef __linux__

#include <unistd.h>
void rest_for_milliseconds(unsigned int milliseconds) {
    usleep(milliseconds * 1000);
}

#elif _WIN32

#include <windows.h>
void rest_for_milliseconds(unsigned int milliseconds) {
    Sleep(milliseconds);
}

#else

#error Platform not supported!

#endif

#endif
