#ifndef PHYSICS_H
#define PHYSICS_H

#include "box.h"
#include "command.h"
#include "game.h"
#include "platform.h"
#include "player.h"
#include <stdlib.h>

/**
 * From an array of lines occupancy states, selects at random an empty line.
 *
 * If no such line exists, returns a random line.
 *
 * This algorithm is O(n) with respect to the number of lines.
 */
int select_random_line_blindly(const unsigned char *lines, const int size);

/**
 * From an array of lines occupancy states, selects at random one of the lines
 * which are the furthest away from any other occupied line.
 *
 * This algorithm is O(n) with respect to the number of lines.
 */
int select_random_line_awarely(const unsigned char *lines, const int size);

void update_platforms(Game *game);

void update_perk(Game *game);

void update_player(Game *game, Command command);

/**
 * Conceives a bonus perk to the player.
 */
void conceive_bonus(Player *player, Perk perk);

#endif
