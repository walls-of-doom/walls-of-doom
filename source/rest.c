#define ONE_BILLION 1000000000

#include <time.h>

void rest_for_ns(long ns) {
    if (ns < 1) {
        return;
    }
    struct timespec sleep_duration;
    sleep_duration.tv_sec = ns / ONE_BILLION;
    sleep_duration.tv_nsec = ns % ONE_BILLION;
    struct timespec remaining;
    nanosleep(&sleep_duration, &remaining);
    // Currently we quietly ignore if the sleep was interrupted.
}

/**
 * Rests for a number of seconds equal to the reciprocal of the provided
 * argument.
 */
void rest_for_second_fraction(int fps) {
    if (fps < 1) {
        return;
    }
    rest_for_ns(ONE_BILLION / fps);
}
