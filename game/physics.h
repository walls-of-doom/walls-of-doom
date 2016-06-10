#ifndef PHYSICS_H
#define PHYSICS_H

#include "command.h"
#include "game.h"
#include "platform.h"
#include "player.h"
#include "types.h"

#include "stdlib.h"

int bounding_box_equals(const BoundingBox * const a, const BoundingBox * const b);

void update_platforms(Game * const game);

void update_perk(Game * const game);

void update_player(Game * const game, const Command command);

/**
 * Conceives a bonus perk to the player.
 */
void conceive_bonus(Player * const player, Perk perk);

#endif
