#ifndef LOGGER_H
#define LOGGER_H

#include "score.h"

/**
 * Initializes the logger. Should only be called once.
 */
void initialize_logger(void);

/**
 * Terminates the logger. Should only be called once.
 */
void finalize_logger(void);

/**
 * Logs the provided message to the current log file.
 */
void log_message(const char *message);

void log_player_score(const unsigned long frame, const Score score);

#endif
