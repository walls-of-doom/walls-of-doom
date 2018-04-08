#ifndef CLOCK_H
#define CLOCK_H

#include "integers.hpp"
#include <chrono>

using Milliseconds = U64;

using TimePoint = std::chrono::steady_clock::time_point;

/**
 * Returns a number of milliseconds.
 *
 * This function should be used to measure computation times.
 */
Milliseconds get_milliseconds();

/**
 * Sleeps for the specified number of milliseconds or more.
 */
void sleep_milliseconds(Milliseconds amount);

#endif
