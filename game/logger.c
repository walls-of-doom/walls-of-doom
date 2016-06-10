#include "logger.h"

#include <stdio.h>
#include <time.h>

#define TIMESTAMP_BUFFER_SIZE 64

/**
 * Initializes the logger. Should only be called once.
 */
void initialize_logger(void) {
    /* Currently, the logger does not need initialization. */
    log_message("Initialized the logger");
}

/**
 * Terminates the logger. Should only be called once.
 */
void finalize_logger(void) {
    /* Currently, the logger does not need termination. */
    log_message("Finalized the logger");
}

void write_timestamp(char *buffer, const size_t buffer_size) {
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(buffer, buffer_size, "%F %T", time_info);
}

/**
 * Logs the provided message to the current log file.
 */
void log_message(const char *message) {
    FILE *file = fopen("log.txt", "a");
    if (file) {
        char buffer[TIMESTAMP_BUFFER_SIZE];
        write_timestamp(buffer, TIMESTAMP_BUFFER_SIZE);
        fprintf(file, "[%s] %s\n", buffer, message);
        fclose(file);
    }
}
