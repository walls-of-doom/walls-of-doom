#ifndef GAME_H
#define GAME_H

#include "box.h"
#include "logger.h"
#include "math.h"
#include "perks.h"
#include "platform.h"
#include "player.h"
#include "random.h"
#include "vector.h"

#include <stdlib.h>

typedef struct Game {

    Player *player;

    Platform *platforms;
    size_t platform_count;

    unsigned long frame;

    Perk perk;
    Vector perk_position;
    unsigned long perk_end_frame;

    BoundingBox *box;

} Game;

/**
 * Creates a new Game object with the provided objects.
 */
Game create_game(Player *player, Platform *platforms, const size_t platform_count, BoundingBox *box);

void update_game(Game * const game);

/**
 * Runs the main loop of the provided game and registers the player score at the end.
 *
 * Returns 0 if successful.
 */
int run_game(Game * const game);

#endif
