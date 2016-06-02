#ifndef PHYSICS_H
#define PHYSICS_H

#include "box.h"
#include "command.h"
#include "game.h"
#include "platform.h"
#include "player.h"
#include "stdlib.h"

int bounding_box_equals(const BoundingBox * const a, const BoundingBox * const b);

void update_platforms(Game * const game);

void update_perk(Game * const game);

void update_player(Game * const game, const Command command);

#endif
