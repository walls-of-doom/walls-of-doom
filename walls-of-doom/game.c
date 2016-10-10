#include "game.h"

#include "about.h"
#include "constants.h"
#include "data.h"
#include "io.h"
#include "logger.h"
#include "menu.h"
#include "physics.h"
#include "random.h"
#include "record.h"
#include "text.h"
#include "version.h"

#include <stdlib.h>
#include <string.h>

#include <SDL.h>

/**
 * Creates a new Game object with the provided objects.
 */
Game create_game(Player *player, Platform *platforms,
                 const size_t platform_count, BoundingBox *box) {
  Game game;

  game.player = player;
  game.platforms = platforms;
  game.platform_count = platform_count;

  game.frame = 0;
  game.played_frames = 0;

  game.box = box;

  game.perk = PERK_NONE;
  game.perk_x = 0;
  game.perk_y = 0;
  /* Don't start with a Perk on the screen. */
  game.perk_end_frame = PERK_SCREEN_DURATION_IN_FRAMES;

  game.message[0] = '\0';
  game.message_end_frame = 0;
  game.message_priority = 0;

  log_message("Finished creating the game");

  return game;
}

void game_update(Game *const game) {
  if (game->message_end_frame < game->frame) {
    game->message[0] = '\0';
  }
}

/**
 * Changes the game message to the provided text, for the provided duration.
 *
 * If there is a message and it has higher or equal priority, it is unchanged.
 *
 * This function prevents buffer overflow by truncating the message.
 */
void game_set_message(Game *const game, const char *message,
                      const unsigned long duration,
                      const unsigned int priority) {
  int last_message_expired = game->message_end_frame <= game->frame;
  int last_message_has_lower_priority = game->message_priority < priority;
  if (last_message_expired || last_message_has_lower_priority) {
    game->message_end_frame = game->frame + duration * FPS;
    game->message_priority = priority;
    copy_string(game->message, message, MAXIMUM_STRING_SIZE);
  }
}

void register_score(const Game *const game, SDL_Renderer *renderer) {
  const Player *const player = game->player;
  /* Log that we are registering the score */
  char buffer[MAXIMUM_STRING_SIZE];
  const char *format = "Started registering a score of %d points for %s";
  Record record;
  int scoreboard_index;
  int position;
  sprintf(buffer, format, player->score, player->name, renderer);
  log_message(buffer);

  /* The name has already been entered to make the Player object. */
  record = make_record(player->name, player->score);

  /* Write the Record to disk */
  scoreboard_index = save_record(&record);
  position = scoreboard_index + 1;

  sprintf(buffer, "Saved the record successfully");
  log_message(buffer);

  print_game_result(player->name, player->score, position, renderer);
  wait_for_input();
}

/**
 * Runs the main loop of the provided game and registers the player score at the
 * end.
 *
 * Returns 0 if successful.
 */
int run_game(Game *const game, SDL_Renderer *renderer) {
  unsigned long next_played_frames_score = FPS;
  const Milliseconds interval = 1000 / FPS;
  Milliseconds drawing_delta = 0;
  Command command = COMMAND_NONE;
  while (command != COMMAND_QUIT && game->player->lives != 0) {
    /* Game loop */
    if (game->played_frames == next_played_frames_score) {
      game->player->score++;
      next_played_frames_score += FPS;
    }
    game_update(game);
    update_platforms(game);
    update_perk(game);
    drawing_delta = draw_game(game, renderer);
    /* Delay, if needed */
    if (drawing_delta < interval) {
      SDL_Delay(interval - drawing_delta);
    }
    command = read_next_command();
    update_player(game, command);
    game->frame++;
  }
  /* Ignoring how the game ended (quit command, screen resize, or death),
   * register the score */
  register_score(game, renderer);
  return 0;
}
