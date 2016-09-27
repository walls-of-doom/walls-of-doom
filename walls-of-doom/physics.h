#ifndef PHYSICS_H
#define PHYSICS_H

#include "box.h"
#include "command.h"
#include "game.h"
#include "platform.h"
#include "player.h"

#include "stdlib.h"

int bounding_box_equals(const BoundingBox *const a, const BoundingBox *const b);

void update_platforms(Game *game);

void update_perk(Game *game);

void update_player(Game *game, Command command);

/**
 * Conceives a bonus perk to the player.
 */
void conceive_bonus(Player *player, Perk perk);

#endif
