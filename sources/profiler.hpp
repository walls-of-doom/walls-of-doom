#ifndef PROFILER_H
#define PROFILER_H

#include "clock.hpp"
#include "code.hpp"

Code initialize_profiler();

/**
 * Updates the statistics about an identifier with a new millisecond count.
 */
void update_profiler(const char *identifier, Milliseconds delta);

/**
 * Begins the profiling of the execution of the provided identifier.
 */
void profiler_begin(const char *identifier);

/**
 * Ends the profiling of the execution of the provided identifier.
 */
void profiler_end(const char *identifier);

/**
 * Saves all profiler data to disk and frees the allocated memory.
 */
Code finalize_profiler();

#endif
