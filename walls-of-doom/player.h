#ifndef PLAYER_H
#define PLAYER_H

#include "investment.h"
#include "perk.h"
#include "score.h"
#include "graphics.h"

typedef struct Player {

  char *name;

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
Player make_player(char *name);

void player_score_add(Player *player, const Score score);

void player_score_sub(Player *player, const Score score);

#endif
