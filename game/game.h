#ifndef GAME_H
#define GAME_H

#include "logger.h"
#include "math.h"
#include "perks.h"
#include "platform.h"
#include "player.h"
#include "random.h"
#include "types.h"

#include <stdlib.h>

#include <SDL.h>

typedef struct Game {

  Player *player;

  Platform *platforms;
  size_t platform_count;

  /**
   * In which frame - starting at 0 - we are now.
   */
  unsigned long frame;

  /**
   * Count of frames on which the player was active (not in the initial state).
   */
  unsigned long played_frames;

  Perk perk;
  int perk_x;
  int perk_y;
  unsigned long perk_end_frame;

  BoundingBox *box;

} Game;

/**
 * Creates a new Game object with the provided objects.
 */
Game create_game(Player *player, Platform *platforms,
                 const size_t platform_count, BoundingBox *box);

void update_game(Game *const game);

/**
 * Runs the main loop of the provided game and registers the player score at the
 * end.
 *
 * Returns 0 if successful.
 */
int run_game(Game *const game, SDL_Renderer *renderer);

#endif
