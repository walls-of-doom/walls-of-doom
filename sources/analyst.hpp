#ifndef ANALYST_H
#define ANALYST_H

#include "game.hpp"
#include "integers.hpp"

/**
 * Returns a normalized value in [0.0, 1.0] indicating the game difficulty.
 */
F64 get_difficulty(const Game &game);

#endif
