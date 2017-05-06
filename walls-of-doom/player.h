#ifndef PLAYER_H
#define PLAYER_H

#include "command.h"
#include "graphics.h"
#include "investment.h"
#include "perk.h"
#include "score.h"

typedef struct Player {

  char *name;

  CommandTable *table;

  int x;
  int y;
  int w;
  int h;
  int speed_x;
  int speed_y;

  /* Whether or not the player is being affected by physics. */
  int physics;

  int can_double_jump;
  int remaining_jump_height;

  int lives;

  Score score;

  Perk perk;
  unsigned long perk_end_frame;

  Investment *investments;

  Graphics *graphics;

} Player;

/**
 * Returns an initialized Player object with the provided name.
 *
 * An initialized Player object is an object which is ready to start a game.
 */
Player create_player(char name[64], CommandTable *pTable);
void destroy_player(Player *player);

void player_score_add(Player *player, const Score score);

void player_score_sub(Player *player, const Score score);

#endif
