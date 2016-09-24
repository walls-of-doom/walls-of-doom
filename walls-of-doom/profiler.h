#ifndef PROFILER_H
#define PROFILER_H

#include "clock.h"
#include "codes.h"

Code initialize_profiler(void);

/**
 * Updates the statistics about an identifier with a new millisecond count.
 */
void update_profiler(const char *identifier, const Milliseconds delta);

/**
 * Saves all profiler data to disk and frees the allocated memory.
 */
Code finalize_profiler(void);

#endif
