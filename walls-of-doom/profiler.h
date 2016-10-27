#ifndef PROFILER_H
#define PROFILER_H

#include "clock.h"
#include "code.h"

Code initialize_profiler(void);

/**
 * Updates the statistics about an identifier with a new millisecond count.
 */
void update_profiler(const char *identifier, const Milliseconds delta);

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
Code finalize_profiler(void);

#endif
