#include "game.h"
#include "about.h"
#include "box.h"
#include "constants.h"
#include "data.h"
#include "high-io.h"
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

#define DEFAULT_LIMIT_PLAYED_MINUTES 2
#define DEFAULT_LIMIT_PLAYED_SECONDS DEFAULT_LIMIT_PLAYED_MINUTES * 60
#define DEFAULT_LIMIT_PLAYED_FRAMES DEFAULT_LIMIT_PLAYED_SECONDS *FPS

void print_command_table(CommandTable *table);

char *command_to_string(Command command);

static size_t get_rigid_matrix_index(const Game *const game, const int x, const int y) {
  const int base_x = x - game->box->min_x;
  const int base_y = y - game->box->min_y;
  return base_x + base_y * game->rigid_matrix_n;
}

unsigned char get_from_rigid_matrix(const Game *const game, const int x, const int y) {
  if (bounding_box_contains(game->box, x, y)) {
    return game->rigid_matrix[get_rigid_matrix_index(game, x, y)];
  }
  return 0;
}

void modify_rigid_matrix_point(const Game *const game, const int x, const int y, const unsigned char delta) {
  if (bounding_box_contains(game->box, x, y)) {
    game->rigid_matrix[get_rigid_matrix_index(game, x, y)] += delta;
  }
}

void modify_rigid_matrix_platform(Game *game, Platform const *platform, const unsigned char delta) {
  int x;
  int y;
  for (x = 0; x < platform->w; ++x) {
    for (y = 0; y < platform->h; ++y) {
      modify_rigid_matrix_point(game, platform->x + x, platform->y + y, delta);
    }
  }
}

static void initialize_rigid_matrix(Game *game) {
  size_t i;
  memset(game->rigid_matrix, 0, game->rigid_matrix_size);
  for (i = 0; i < game->platform_count; i++) {
    modify_rigid_matrix_platform(game, game->platforms + i, 1);
  }
}

static void initialize_bounding_box(Game *game) {
  game->box->min_x = 0;
  game->box->min_y = 0;
  game->box->max_x = get_window_width();
  game->box->max_y = get_window_height() - 2 * get_bar_height();
}

/**
 * Creates a new Game object with the provided objects.
 */
Game create_game(Player *player) {
  const int tile_w = get_tile_width();
  const int tile_h = get_tile_height();
  const int platform_count = get_platform_count();
  size_t rigid_matrix_bytes;
  Game game;

  game.player = player;
  game.platform_count = platform_count;
  game.platforms = resize_memory(NULL, sizeof(Platform) * platform_count);

  game.frame = 0;
  game.played_frames = 0;
  game.limit_played_frames = DEFAULT_LIMIT_PLAYED_FRAMES;

  game.paused = 0;

  game.tile_w = tile_w;
  game.tile_h = tile_h;

  player->w = tile_w;
  player->h = tile_h;

  game.box = resize_memory(NULL, sizeof(BoundingBox));
  initialize_bounding_box(&game);

  generate_platforms(game.platforms, game.box, platform_count, tile_w, tile_h);

  reposition_player(&game);

  game.perk = PERK_NONE;
  game.perk_x = 0;
  game.perk_y = 0;
  /* Don't start with a Perk on the screen. */
  game.perk_end_frame = PERK_SCREEN_DURATION_IN_FRAMES;

  game.rigid_matrix_m = game.box->max_y - game.box->min_y + 1;
  game.rigid_matrix_n = game.box->max_x - game.box->min_x + 1;
  game.rigid_matrix_size = game.rigid_matrix_m * game.rigid_matrix_n;
  rigid_matrix_bytes = sizeof(unsigned char) * game.rigid_matrix_size;
  game.rigid_matrix = resize_memory(NULL, rigid_matrix_bytes);
  initialize_rigid_matrix(&game);

  game.message[0] = '\0';
  game.message_end_frame = 0;
  game.message_priority = 0;

  log_message("Finished creating the game.");

  return game;
}

void destroy_game(Game *game) {
  destroy_player(game->player);
  game->rigid_matrix = resize_memory(game->rigid_matrix, 0);
  game->box = resize_memory(game->box, 0);
  game->platforms = resize_memory(game->platforms, 0);
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
void game_set_message(Game *const game, const char *message, const unsigned long duration,
                      const unsigned int priority) {
  const int last_has_expired = game->message_end_frame <= game->frame;
  const int last_has_lower_priority = game->message_priority <= priority;
  if (last_has_expired || last_has_lower_priority) {
    game->message_end_frame = game->frame + duration * FPS;
    game->message_priority = priority;
    copy_string(game->message, message, MAXIMUM_STRING_SIZE);
  }
}

static void print_game_result(const Player *player, const int position, SDL_Renderer *renderer) {
  const char *name = player->name;
  const Score score = player->score;
  const ColorPair color = COLOR_PAIR_DEFAULT;
  char first_line[MAXIMUM_STRING_SIZE];
  char second_line[MAXIMUM_STRING_SIZE];
  char *lines[3];
  lines[0] = first_line;
  lines[1] = "";
  lines[2] = second_line;
  sprintf(first_line, "%s died after making %ld points.", name, score);
  if (position > 0) {
    sprintf(second_line, "%s got to position %d!", name, position);
  } else {
    sprintf(second_line, "%s didn't make it to the top scores.", name);
  }
  clear(renderer);
  print_centered_vertically(3, lines, color, renderer);
  present(renderer);
}

void register_score(const Game *const game, SDL_Renderer *renderer) {
  const Player *const player = game->player;
  char buffer[MAXIMUM_STRING_SIZE];
  const char *format = "Started registering a score of %d points for %s.";
  Record record;
  int scoreboard_index;
  int position;
  sprintf(buffer, format, player->score, player->name, renderer);
  log_message(buffer);
  record = make_record(player->name, player->score);
  scoreboard_index = save_record(&record);
  position = scoreboard_index + 1;
  log_message("Saved the record successfully.");
  print_game_result(player, position, renderer);
  wait_for_input(game->player->table);
}

/**
 * Runs the main game loop for the Game object and registers the player score.
 */
Code run_game(Game *const game, SDL_Renderer *renderer) {
  unsigned long next_played_frames_score = FPS;
  const Milliseconds interval = 1000 / FPS;
  Milliseconds drawing_delta = 0;
  Milliseconds updating_delta = 0;
  Code code = CODE_OK;
  int *lives = &game->player->lives;
  unsigned long *played = &game->played_frames;
  unsigned long limit = game->limit_played_frames;
  CommandTable table;
  initialize_command_table(&table);
  while (!game->player->table->status[COMMAND_QUIT] && *lives != 0 && *played < limit) {
    if (game->paused) {
      drawing_delta = draw_game(game, renderer);
      if (drawing_delta < interval) {
        sleep_milliseconds(interval - drawing_delta);
      }
      read_commands(game->player->table);
      if (test_command_table(game->player->table, COMMAND_CLOSE, 200)) {
        code = CODE_CLOSE;
      } else if (test_command_table(game->player->table, COMMAND_CLOSE, 200)) {
        code = CODE_QUIT;
      }
      if (test_command_table(game->player->table, COMMAND_PAUSE, 200)) {
        game->paused = 0;
      }
      continue;
    }
    if (game->played_frames == next_played_frames_score) {
      player_score_add(game->player, 1);
      next_played_frames_score += FPS;
    }
    updating_delta = update_game(game);
    drawing_delta = draw_game(game, renderer);
    if (updating_delta + drawing_delta < interval) {
      sleep_milliseconds(interval - updating_delta - drawing_delta);
    }
    read_commands(game->player->table);
    update_player(game, game->player);
    game->frame++;
    if (test_command_table(game->player->table, COMMAND_PAUSE, 200)) {
      game->paused = 1;
    }
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
