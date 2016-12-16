#include "player.h"
#include "logger.h"
#include <limits.h>
#include <stdlib.h>

#define MAXIMUM_PLAYER_SCORE LONG_MAX
#define MINIMUM_PLAYER_SCORE 0

/**
 * Returns an initialized Player object with the provided name.
 *
 * An initialized Player object is an object which is ready to start a game.
 */
Player make_player(char *name) {
  Player player;
  player.name = name;
  /* Initialize the player to the corner so that it is in a valid state. */
  player.x = 0;
  player.y = 0;
  player.w = 0;
  player.h = 0;
  player.speed_x = 0;
  player.speed_y = 0;
  player.physics = 0;
  player.can_double_jump = 0;
  player.remaining_jump_height = 0;
  player.lives = 3;
  player.score = 0;
  player.perk = PERK_NONE;
  player.perk_end_frame = 0;
  player.investments = NULL;
  player.graphics = NULL;
  return player;
}

void player_score_add(Player *player, const Score score) {
  const Score maximum_add = MAXIMUM_PLAYER_SCORE - player->score;
  if (maximum_add >= score) {
    player->score += score;
  } else {
    log_message("Prevented Player score overflow!");
    player->score = MAXIMUM_PLAYER_SCORE;
  }
}

void player_score_sub(Player *player, const Score score) {
  const Score maximum_sub = player->score - MINIMUM_PLAYER_SCORE;
  if (maximum_sub >= score) {
    player->score -= score;
  } else {
    log_message("Prevented Player score underflow!");
    player->score = MINIMUM_PLAYER_SCORE;
  }
}
