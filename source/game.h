#ifndef GAME_H
#define GAME_H

#include "box.h"
#include "logger.h"
#include "math.h"
#include "platform.h"
#include "player.h"
#include "random.h"

typedef struct Game {
    Player *player;
    Platform *platforms;
    size_t platform_count;
    BoundingBox *box;
} Game;

void update_game(Game * const game);
void update_platforms(Game * const game);
void update_player(Game * const game);

#endif
