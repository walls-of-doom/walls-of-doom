#ifndef PLAYER_H
#define PLAYER_H

#include "perks.h"

typedef struct Player {

  char *name;

  int x;
  int y;
  int speed_x;
  int speed_y;

  /* Whether or not the player is being affected by physics. */
  int physics;

  int can_double_jump;
  int remaining_jump_height;

  int lives;
  int score;

  Perk perk;
  unsigned long perk_end_frame;

} Player;

/**
 * Returns an initialized Player object with the provided name.
 *
 * An initialized Player object is an object which is ready to start a game.
 */
Player make_player(char *name);

#endif
