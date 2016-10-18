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
 * Updates the statistics about an identifier with a new nanosecond count.
 */
void update_profiler_precise(const char *identifier, const Nanoseconds delta);

/**
 * Saves all profiler data to disk and frees the allocated memory.
 */
Code finalize_profiler(void);

#endif
