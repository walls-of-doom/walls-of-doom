#ifndef LOGGER_H
#define LOGGER_H

#include "score.hpp"
#include <string>

/**
 * Initializes the logger. Should only be called once.
 */
void initialize_logger();

/**
 * Terminates the logger. Should only be called once.
 */
void finalize_logger();

/**
 * Logs the provided message to the current log file.
 */
void log_message(const std::string &message);

void log_player_score(const U64 frame, const Score score);

#endif
