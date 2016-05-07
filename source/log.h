#ifndef LOG_H
#define LOG_H

#include <stdio.h>

void log_message(const char *message) {
    FILE *file = fopen("log.txt", "a");
    if (file) {
        fprintf(file, message);
        fclose(file);
    }
}

#endif
