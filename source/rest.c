#include "rest.h"

#include "logger.h"

#include <time.h>

/**
 * Returns the nanoseconds difference between the two provided timespecs.
 */
uint_least64_t elapsed_time_in_nanoseconds(const struct timespec * const start, const struct timespec * const end) {
    if (end->tv_sec < start->tv_sec || (end->tv_sec == start->tv_sec && end->tv_nsec < start->tv_nsec)) {
        log_message("elapsed_time_in_nanoseconds got an end time greater than the start time");
        return 0;
    }
    // Let x = (end->tv_sec - start->tv_sec) * NANOSECONDS_IN_ONE_SECOND
    // If x is 0, then end->tv_nsec >= start->tv_nsec and subtracting is safe even for unsigned types.
    // Otherwise, then it is bigger than start->tv_nsec and subtracting is fine even for unsigned types.
    return (end->tv_sec - start->tv_sec) * NANOSECONDS_IN_ONE_SECOND + end->tv_nsec - start->tv_nsec;
}

/**
 * Rests for the specified number of nanoseconds.
 */
void rest_for_nanoseconds(uint_least64_t nanoseconds) {
    struct timespec sleep_duration;
    sleep_duration.tv_sec = nanoseconds / NANOSECONDS_IN_ONE_SECOND;
    sleep_duration.tv_nsec = nanoseconds % NANOSECONDS_IN_ONE_SECOND;
    struct timespec remaining;
    nanosleep(&sleep_duration, &remaining);
    // Currently we quietly ignore if the sleep was interrupted.
}

/**
 * Rests for the reciprocal of the provided argument seconds.
 */
void rest_for_second_fraction(int fps) {
    if (fps < 1) {
        return;
    }
    rest_for_nanoseconds(NANOSECONDS_IN_ONE_SECOND / fps);
}
