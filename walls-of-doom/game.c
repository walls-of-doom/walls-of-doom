#include "game.h"
#include "about.h"
#include "box.h"
#include "constants.h"
#include "data.h"
#include "io.h"
#include "logger.h"
#include "memory.h"
#include "menu.h"
#include "physics.h"
#include "profiler.h"
#include "random.h"
#include "record.h"
#include "text.h"
#include "version.h"
#include <SDL.h>
#include <stdlib.h>
#include <string.h>

static size_t get_rigid_matrix_index(const Game *const game, const int x,
                                     const int y) {
  const int base_x = x - game->box->min_x;
  const int base_y = y - game->box->min_y;
  return base_x + base_y * game->rigid_matrix_m;
}

unsigned char get_from_rigid_matrix(const Game *const game, const int x,
                                    const int y) {
  if (bounding_box_contains(game->box, x, y)) {
    return game->rigid_matrix[get_rigid_matrix_index(game, x, y)];
  }
  return 0;
}

void modify_rigid_matrix_point(const Game *const game, const int x, const int y,
                               const unsigned char delta) {
  if (bounding_box_contains(game->box, x, y)) {
    game->rigid_matrix[get_rigid_matrix_index(game, x, y)] += delta;
  }
}

void modify_rigid_matrix_platform(Game *game, Platform const *platform,
                                  const unsigned char delta) {
  int i;
  for (i = 0; i < platform->width; i++) {
    modify_rigid_matrix_point(game, platform->x + i, platform->y, delta);
  }
}

static void initialize_rigid_matrix(Game *game) {
  size_t i;
  memset(game->rigid_matrix, 0, game->rigid_matrix_size);
  for (i = 0; i < game->platform_count; i++) {
    modify_rigid_matrix_platform(game, game->platforms + i, 1);
  }
}

/**
 * Creates a new Game object with the provided objects.
 */
Game create_game(Player *player, Platform *platforms,
                 const size_t platform_count, BoundingBox *box) {
  size_t rigit_matrix_bytes;
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

  game.rigid_matrix_n = box->max_y - box->min_y + 1;
  game.rigid_matrix_m = box->max_x - box->min_x + 1;
  game.rigid_matrix_size = game.rigid_matrix_n * game.rigid_matrix_m;
  game.rigid_matrix = NULL;
  rigit_matrix_bytes = sizeof(unsigned char) * game.rigid_matrix_size;
  game.rigid_matrix = resize_memory(game.rigid_matrix, rigit_matrix_bytes);
  initialize_rigid_matrix(&game);

  game.message[0] = '\0';
  game.message_end_frame = 0;
  game.message_priority = 0;

  log_message("Finished creating the game");

  return game;
}

void destroy_game(Game *game) {
  game->rigid_matrix = resize_memory(game->rigid_matrix, 0);
}

Milliseconds update_game(Game *const game) {
  Milliseconds game_update_start;
  profiler_begin("update_game");
  game_update_start = get_milliseconds();
  if (game->message_end_frame < game->frame) {
    game->message[0] = '\0';
  }
  update_platforms(game);
  update_perk(game);
  profiler_end("update_game");
  return get_milliseconds() - game_update_start;
}

/**
 * Changes the game message to the provided text, for the provided duration.
 *
 * If there is a message and it has higher priority, it is not changed.
 *
 * This function prevents buffer overflow by truncating the message.
 */
void game_set_message(Game *const game, const char *message,
                      const unsigned long duration,
                      const unsigned int priority) {
  int last_message_expired = game->message_end_frame <= game->frame;
  int last_message_has_lower_priority = game->message_priority <= priority;
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

static Code code_from_command(const Command command) {
  if (command == COMMAND_CLOSE) {
    return CODE_CLOSE;
  } else if (command == COMMAND_QUIT) {
    return CODE_QUIT;
  } else {
    return CODE_OK;
  }
}

/**
 * Runs the main game loop for the Game object and registers the player score.
 */
Code run_game(Game *const game, SDL_Renderer *renderer) {
  unsigned long next_played_frames_score = FPS;
  const Milliseconds interval = 1000 / FPS;
  Milliseconds drawing_delta = 0;
  Milliseconds updating_delta = 0;
  Command command = COMMAND_NONE;
  Code code = CODE_OK;
  while (!is_termination_code(code) && game->player->lives != 0) {
    if (game->played_frames == next_played_frames_score) {
      game->player->score++;
      next_played_frames_score += FPS;
    }
    updating_delta = update_game(game);
    drawing_delta = draw_game(game, renderer);
    /* Delay, if needed. */
    if (updating_delta + drawing_delta < interval) {
      sleep(interval - updating_delta - drawing_delta);
    }
    command = read_next_command();
    code = code_from_command(command);
    update_player(game, command);
    game->frame++;
  }
  if (code != CODE_CLOSE) {
    register_score(game, renderer);
  }
  if (code == CODE_QUIT) {
    /* When the player quits from the game, it should go back to the menu. */
    code = CODE_OK;
  }
  return code;
}
