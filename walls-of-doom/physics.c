#include "physics.h"
#include "bank.h"
#include "base-io.h"
#include "constants.h"
#include "investment.h"
#include "limits.h"
#include "logger.h"
#include "memory.h"
#include "profiler.h"
#include "random.h"
#include "score.h"
#include "settings.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

/* Should be the maximum frame count value for 5 seconds remaining. */
#define MINIMUM_REMAINING_FRAMES_FOR_MESSAGE (6 * FPS - 1)

/* Extra level of indirection needed to expand macros before the conversion. */
#define AS_STR(X) #X
#define STR(X) AS_STR(X)

#define BUY_LIFE_PRICE 100
#define BUY_LIFE_FORMAT(PRICE) "Bought an extra life for " STR(PRICE) " points."
#define BUY_LIFE_MESSAGE BUY_LIFE_FORMAT(BUY_LIFE_PRICE)

static BoundingBox derive_box(const Game *game, const int x, const int y) {
  BoundingBox box;
  box.min_x = x;
  box.min_y = y;
  box.max_x = box.min_x + game->tile_w - 1;
  box.max_y = box.min_y + game->tile_h - 1;
  return box;
}

static int has_rigid_support(const Game *game, int x, int y, int w, int h) {
  int i;
  for (i = 0; i < w; i++) {
    if (get_from_rigid_matrix(game, x + i, y + h)) {
      return 1;
    }
  }
  return 0;
}

static int is_over_platform(const Player *player, const Platform *const platform) {
  if (player->y + player->h == platform->y) {
    if (player->x < platform->x + platform->w) {
      return player->x + player->w > platform->x;
    }
  }
  return 0;
}

/* Width and height are the width and height of the matrix tile. */
static int violates_rigid_matrix(const Game *game, int x, int y, int w, int h) {
  int i;
  int j;
  for (i = 0; i < w; i++) {
    for (j = 0; j < h; j++) {
      if (get_from_rigid_matrix(game, x + i, y + j)) {
        return 1;
      }
    }
  }
  return 0;
}

/**
 * Evaluates whether or not the given x and y pair is a valid position for the
 * player to occupy.
 */
static int is_valid_move(const Game *const game, const int x, const int y) {
  if (game->player->perk == PERK_POWER_INVINCIBILITY) {
    /* If it is invincible, it shouldn't move into walls. */
    if (x == game->box->min_x - 1) {
      return 0;
    }
    if (x + game->player->w - 1 == game->box->max_x + 1) {
      return 0;
    } else if (y == game->box->min_y - 1) {
      return 0;
    } else if (y + game->player->h - 1 == game->box->max_y + 1) {
      return 0;
    }
  }
  return !violates_rigid_matrix(game, x, y, game->player->w, game->player->h);
}

/**
 * Moves the player by the provided x and y directions.
 *
 * This moves the player at most one position on each axis.
 */
static void move_player(Game *game, int x, int y) {
  /* It is OK to reuse x and y to prevent multiple integers for the same axis.
   */
  /* Ignore magnitude, take just -1, 0, or 1. */
  x = normalize(x);
  y = normalize(y);
  /* Just in case a compiler cannot optimize this case away. */
  if (x == 0 && y == 0) {
    return;
  }
  if (is_valid_move(game, game->player->x + x, game->player->y + y)) {
    game->player->x += x;
    game->player->y += y;
  }
}

/**
 * Attempts to force the Player to move according to the provided displacement.
 *
 * If the player does not have physics enabled, this is a no-op.
 *
 * The standing flag indicates if the player is standing above the platform.
 */
static void shove_player(Game *game, int x, int y, int standing) {
  if (game->player->physics) {
    /* Don't shove the player if he is hovering over a platform. */
    if (game->player->perk != PERK_POWER_LEVITATION || !standing) {
      move_player(game, x, 0);
    }
  }
  move_player(game, 0, y);
}

static int get_absolute_pending_movement(unsigned long frame, int speed) {
  /* Should move slice after every frame. */
  const double slice = speed / (double)FPS;
  /* To reduce floating point error, normalize frame to [FPS, 2 FPS - 1]. */
  frame = frame % FPS + FPS;
  return floor(frame * slice) - floor((frame - 1) * slice);
}

static int get_pending_movement(const Game *const game, const int speed) {
  const int normalized = normalize(speed);
  return normalized * get_absolute_pending_movement(game->frame, abs(speed));
}

static void subtract_platform(Game *const game, Platform *const platform) {
  modify_rigid_matrix_platform(game, platform, -1);
}

static void add_platform(Game *const game, Platform *const platform) {
  modify_rigid_matrix_platform(game, platform, 1);
}

static int is_free_on_matrix(Game *const game, int x, int y, int w, int h) {
  int i;
  int j;
  for (i = 0; i != w; i++) {
    for (j = 0; j != h; j++) {
      if (get_from_rigid_matrix(game, x + i, y + j)) {
        return 0;
      }
    }
  }
  return 1;
}

/**
 * Returns whether or not the platform can be inserted in the game without
 * overlapping any existing platforms.
 */
static int can_insert_platform(Game *const game, Platform *const p) {
  return is_free_on_matrix(game, p->x, p->y, p->w, p->h);
}

/**
 * Evaluates whether or not the player is standing on a platform.
 *
 * This function takes into account the Invincibility perk, which makes the
 * bottom border to be treated as a platform.
 */
static int is_standing_on_platform(const Game *const game) {
  const int x = game->player->x;
  const int y = game->player->y;
  const int w = game->player->w;
  const int h = game->player->h;
  if (y + h - 1 == game->box->max_y) {
    return game->player->perk == PERK_POWER_INVINCIBILITY;
  }
  return has_rigid_support(game, x, y, w, h);
}

static int is_in_front_of_platform(const Player *const player, const Platform *const platform) {
  if (platform->speed < 0) {
    if (player->x + player->w != platform->x) {
      return 0;
    }
  } else {
    if (platform->x + platform->w != player->x) {
      return 0;
    }
  }
  if (player->y < platform->y + platform->h) {
    if (player->y + player->h > platform->y) {
      return 1;
    }
  }
  return 0;
}

static int can_move_platform(Game *const game, Platform *p, int dx, int dy) {
  int can_move = 1;
  if (get_player_stops_platforms() && is_over_platform(game->player, p)) {
    return 0;
  }
  if (dx == 0 && dy == 0) {
    return 1;
  }
  /* There are two optimized paths for unidirectional movement. */
  if (dx == 0) {
    if (dy < 0) {
      return is_free_on_matrix(game, p->x, p->y + dy, p->w, -dy);
    }
    if (dy > 0) {
      return is_free_on_matrix(game, p->x, p->y + p->h, p->w, dy);
    }
  } else if (dy == 0) {
    if (dx < 0) {
      return is_free_on_matrix(game, p->x + dx, p->y, -dx, p->h);
    }
    if (dx > 0) {
      return is_free_on_matrix(game, p->x + p->w, p->y, dx, p->h);
    }
  } else {
    /* If the platform would intersect with another platform, do not move it. */
    subtract_platform(game, p);
    p->x += dx;
    p->y += dy;
    can_move = can_insert_platform(game, p);
    p->x -= dx;
    p->y -= dy;
    add_platform(game, p);
  }
  return can_move;
}

/**
 * This function is the ONLY right way to move a platform.
 *
 * This function keeps the cached rigid body matrix in the Game object valid.
 */
static void move_platform(Game *const game, Platform *const platform, const int dx, const int dy) {
  if (can_move_platform(game, platform, dx, dy)) {
    subtract_platform(game, platform);
    platform->x += dx;
    platform->y += dy;
    add_platform(game, platform);
  }
}

static void move_platform_horizontally(Game *const game, Platform *const platform) {
  const int normalized_speed = normalize(platform->speed);
  /* This could be made more efficient by handling each direction separately. */
  int pending = abs(get_pending_movement(game, platform->speed));
  while (pending) {
    if (can_move_platform(game, platform, normalized_speed, 0)) {
      if (is_in_front_of_platform(game->player, platform)) {
        shove_player(game, normalized_speed, 0, 0);
      }
      if (is_over_platform(game->player, platform)) {
        shove_player(game, normalized_speed, 0, 1);
      }
      move_platform(game, platform, normalized_speed, 0);
    }
    pending--;
  }
}

/**
 * From an array of lines occupancy states, selects at random an empty line.
 *
 * If no such line exists, returns a random line.
 *
 * This algorithm is O(n) with respect to the number of lines.
 */
int select_random_line_blindly(const unsigned char *lines, const int size) {
  int count;
  int skip;
  int line;
  int i;
  /* Be careful not to call random_integer with invalid parameters. */
  if (size < 1) {
    return 0;
  }
  /* Count how many empty lines there are. */
  count = 0;
  for (i = 0; i < size; i++) {
    if (!lines[i]) {
      count++;
    }
  }
  /* No empty lines, return any line. */
  if (count == 0) {
    return random_integer(0, size - 1);
  }
  /* Get a random value based on the count. */
  skip = random_integer(0, count - 1);
  line = 0;
  while (lines[line] || skip != 0) {
    if (!lines[line]) {
      skip--;
    }
    line = (line + 1) % size;
  }
  return line;
}

/**
 * From an array of lines occupancy states, selects at random one of the lines
 * which are the furthest away from any other occupied line.
 *
 * This algorithm is O(n) with respect to the number of lines.
 */
int select_random_line_awarely(const unsigned char *lines, const int size) {
  int *distances = NULL;
  int maximum_distance = INT_MIN;
  int count;
  int skip;
  int line;
  int i;
  /* Be careful not to call random_integer with invalid parameters. */
  if (size < 1) {
    return 0;
  }
  distances = resize_memory(distances, sizeof(int) * size);
  /* First pass: calculate the distance to nearest occupied line above. */
  for (i = 0; i < size; i++) {
    if (lines[i]) {
      distances[i] = 0;
    } else {
      if (i > 0) {
        distances[i] = distances[i - 1] + 1;
      } else {
        distances[i] = 1;
      }
    }
  }
  /* Second pass: calculate the distance to nearest occupied line below. */
  for (i = size - 1; i >= 0; i--) {
    if (lines[i]) {
      distances[i] = 0;
    } else {
      if (i < size - 1) {
        /* Use the minimum distance to first occupied line above or below. */
        distances[i] = min_int(distances[i], distances[i + 1] + 1);
      } else {
        distances[i] = 1;
      }
    }
    maximum_distance = max_int(maximum_distance, distances[i]);
  }
  /* Count how many occurrences of the maximum distance there are. */
  count = 0;
  for (i = 0; i < size; i++) {
    if (distances[i] == maximum_distance) {
      count++;
    }
  }
  /* Get a random value based on the count. */
  skip = random_integer(0, count - 1);
  line = 0;
  while (distances[line] != maximum_distance || skip != 0) {
    if (distances[line] == maximum_distance) {
      skip--;
    }
    line = (line + 1) % size;
  }
  resize_memory(distances, 0);
  return line;
}

static void reposition(Game *const game, Platform *const platform) {
  const BoundingBox *const box = game->box;
  /* The occupied size may be smaller than the array actually is. */
  const int occupied_size = (get_window_height() - 2 * get_bar_height()) / get_tile_height();
  const int tile_h = game->tile_h;
  unsigned char *occupied = NULL;
  int line;
  size_t i;
  occupied = resize_memory(occupied, occupied_size);
  memset(occupied, 0, occupied_size);
  /* Build a table of occupied rows. */
  for (i = 0; i < game->platform_count; i++) {
    if (!platform_equals(game->platforms[i], *platform)) {
      occupied[(game->platforms[i].y - box->min_y) / tile_h] = 1;
    }
  }
  if (get_reposition_algorithm() == REPOSITION_SELECT_BLINDLY) {
    line = select_random_line_blindly(occupied, occupied_size);
  } else {
    line = select_random_line_awarely(occupied, occupied_size);
  }
  resize_memory(occupied, 0);
  if (platform->x > box->max_x) {
    subtract_platform(game, platform);
    /* The platform should be one tick inside the box. */
    platform->x = box->min_x - platform->w + 1;
    platform->y = box->min_y + tile_h * line;
    add_platform(game, platform);
  } else if (platform->x + platform->w < box->min_x) {
    subtract_platform(game, platform);
    /* The platform should be one tick inside the box. */
    platform->x = box->max_x;
    platform->y = box->min_y + tile_h * line;
    add_platform(game, platform);
  }
}

/**
 * Evaluates whether or not a Platform is completely outside of a BoundingBox.
 *
 * Returns 0 if the platform intersects the bounding box.
 * Returns 1 if the platform is to the left or to the right of the bounding box.
 * Returns 2 if the platform is above or below the bounding box.
 */
int is_out_of_bounding_box(Platform *const platform, const BoundingBox *const box) {
  const int min_x = platform->x;
  const int max_x = platform->x + platform->w;
  if (max_x < box->min_x || min_x > box->max_x) {
    return 1;
  }
  if (platform->y < box->min_y || platform->y > box->max_y) {
    return 2;
  } else {
    return 0;
  }
}

static void update_platform(Game *const game, Platform *const platform) {
  move_platform_horizontally(game, platform);
  if (is_out_of_bounding_box(platform, game->box)) {
    reposition(game, platform);
  }
}

void update_platforms(Game *const game) {
  size_t i;
  if (game->player->perk != PERK_POWER_TIME_STOP) {
    for (i = 0; i < game->platform_count; i++) {
      update_platform(game, game->platforms + i);
    }
  }
}

/**
 * Evaluates whether or not the Player is falling.
 */
static int is_falling(const Game *const game) {
  const Player *const player = game->player;
  const int x = player->x;
  const int y = player->y;
  const int w = player->w;
  const int h = player->h;
  if (!player->physics || player->perk == PERK_POWER_LEVITATION) {
    return 0;
  }
  if (y == game->box->max_y) {
    return 1;
  }
  return !has_rigid_support(game, x, y, w, h);
}

static int is_touching_a_wall(const Game *const game) {
  /**
   * As in the maximum values for the boxes, the maximum is the last used pixel.
   */
  const int b_min_x = game->box->min_x;
  const int b_max_x = game->box->max_x;
  const int b_min_y = game->box->min_y;
  const int b_max_y = game->box->max_y;
  const int p_min_x = game->player->x;
  const int p_max_x = game->player->x + game->player->w - 1;
  const int p_min_y = game->player->y;
  const int p_max_y = game->player->y + game->player->h - 1;
  const int in_x = p_min_x < b_min_x || p_max_x > b_max_x;
  const int in_y = p_min_y < b_min_y || p_max_y > b_max_y;
  return in_x || in_y;
}

static int get_bounding_box_center_x(const BoundingBox *const box) {
  return box->min_x + (box->max_x - box->min_x + 1) / 2;
}

static int get_bounding_box_center_y(const BoundingBox *const box) {
  return box->min_y + (box->max_y - box->min_y + 1) / 2;
}

void reposition_player(Game *const game) {
  const BoundingBox *const box = game->box;
  const int x = get_bounding_box_center_x(box);
  const int y = get_bounding_box_center_y(box);
  game->player->x = x;
  game->player->y = y;
}

/**
 * Conceives a bonus perk to the player.
 */
void conceive_bonus(Player *const player, const Perk perk) {
  if (is_bonus_perk(perk)) {
    if (perk == PERK_BONUS_EXTRA_POINTS) {
      player_score_add(player, 60);
    } else if (perk == PERK_BONUS_EXTRA_LIFE) {
      player->lives += 1;
    }
  } else {
    log_message("Called conceive_bonus with a Perk that is not a bonus!");
  }
}

static void accelerate_platform(Platform *const platform) { platform->speed = platform->speed + platform->speed / 2; }

static void reverse_platform(Platform *const platform) { platform->speed = -platform->speed; }

static void apply_to_platforms(Game *const game, void (*f)(Platform *const p)) {
  size_t i;
  for (i = 0; i != game->platform_count; ++i) {
    f(game->platforms + i);
  }
}

/**
 * Process the start of a curse.
 */
void process_curse(Game *const game, const Perk perk) {
  if (is_curse_perk(perk)) {
    if (perk == PERK_CURSE_ACCELERATE_PLATFORMS) {
      apply_to_platforms(game, accelerate_platform);
    } else if (perk == PERK_CURSE_REVERSE_PLATFORMS) {
      apply_to_platforms(game, reverse_platform);
    }
  } else {
    log_message("Called process_curse with a Perk that is not a curse!");
  }
}

void update_perk(Game *const game) {
  unsigned long next_perk_frame = game->perk_end_frame;
  int random_x;
  int random_y;
  next_perk_frame += PERK_INTERVAL_IN_FRAMES;
  next_perk_frame -= PERK_SCREEN_DURATION_IN_FRAMES;
  if (game->played_frames == game->perk_end_frame) {
    game->perk = PERK_NONE;
  } else if (game->played_frames == next_perk_frame) {
    game->perk = get_random_perk();
    random_x = random_integer(0, get_window_width() - get_tile_width());
    random_y = random_integer(get_bar_height(), get_window_height() - 2 * get_bar_height());
    game->perk_x = random_x;
    game->perk_y = random_y - random_y % get_tile_height();
    game->perk_end_frame = game->played_frames + PERK_SCREEN_DURATION_IN_FRAMES;
  }
}

/**
 * Moves the player according to the sign of its current speed if it can move in that direction.
 */
void update_player_horizontal_position(Game *game) {
  int pending_movement = get_pending_movement(game, game->player->speed_x);
  while (pending_movement > 0) {
    move_player(game, 1, 0);
    pending_movement--;
  }
  while (pending_movement < 0) {
    move_player(game, -1, 0);
    pending_movement++;
  }
}

static int is_jumping(const Player *const player) { return player->remaining_jump_height > 0; }

void process_jump(Game *const game) {
  const int jumping_height = game->tile_h * PLAYER_JUMPING_HEIGHT;
  if (is_standing_on_platform(game)) {
    game->player->remaining_jump_height = jumping_height;
    if (game->player->perk == PERK_POWER_SUPER_JUMP) {
      game->player->remaining_jump_height *= 2;
    }
  } else if (game->player->can_double_jump) {
    game->player->can_double_jump = 0;
    game->player->remaining_jump_height += jumping_height / 2;
    if (game->player->perk == PERK_POWER_SUPER_JUMP) {
      game->player->remaining_jump_height *= 2;
    }
  }
}

static void buy_life(Game *game) {
  if (game->player->score >= BUY_LIFE_PRICE) {
    player_score_sub(game->player, BUY_LIFE_PRICE);
    game->player->lives++;
    game_set_message(game, BUY_LIFE_MESSAGE, 1, 1);
  }
}

static void update_player_investments(Game *game) {
  Investment *swap;
  Investment *investments = game->player->investments;
  while (investments != NULL && investments->end <= game->played_frames) {
    player_score_add(game->player, collect_investment(game, *investments));
    swap = investments->next;
    resize_memory(investments, 0);
    investments = swap;
  }
  game->player->investments = investments;
}

static int get_investment_total(Player *player, InvestmentMode mode) {
  const Score base_amount = get_investment_amount();
  const double proportion = get_investment_proportion();
  Score product;
  if (mode == INVESTMENT_MODE_FIXED) {
    return base_amount;
  }
  if (mode == INVESTMENT_MODE_PROPORTIONAL) {
    product = player->score * proportion;
    if (product >= base_amount) {
      return product;
    }
    return base_amount;
  }
  /* Unknown investment type. */
  return 0;
}

static void invest(Game *game, InvestmentMode mode) {
  const int amount = get_investment_total(game->player, mode);
  Investment *investments = game->player->investments;
  Investment *investment = NULL;
  if (amount == 0) {
    return;
  }
  if (game->player->score >= amount) {
    investment = resize_memory(investment, sizeof(Investment));
    player_score_sub(game->player, amount);
    investment->next = NULL;
    investment->amount = amount;
    investment->end = game->played_frames + FPS * get_investment_period();
    while (investments != NULL && investments->next != NULL) {
      investments = investments->next;
    }
    if (investments == NULL) {
      game->player->investments = investment;
    } else {
      investments->next = investment;
    }
  }
}

void process_command(Game *game, Player *player) {
  double *table = player->table->status;
  if (table[COMMAND_LEFT]) {
    double speed = -table[COMMAND_LEFT] * PLAYER_RUNNING_SPEED * game->tile_w;
    player->speed_x = (int)speed;
    player->physics = 1;
  } else if (table[COMMAND_RIGHT]) {
    double speed = table[COMMAND_RIGHT] * PLAYER_RUNNING_SPEED * game->tile_w;
    player->speed_x = (int)speed;
    player->physics = 1;
  } else {
    player->speed_x = (int)0.0;
  }
  if (table[COMMAND_JUMP]) {
    process_jump(game);
    table[COMMAND_JUMP] = 0.0;
    player->physics = 1;
  } else if (table[COMMAND_CONVERT]) {
    buy_life(game);
    table[COMMAND_CONVERT] = 0.0;
  } else if (table[COMMAND_INVEST]) {
    invest(game, get_investment_mode());
    table[COMMAND_INVEST] = 0.0;
  }
}

/**
 * Checks if the character should die and kills it if this is the case.
 */
static void check_for_player_death(Game *game) {
  Player *player = game->player;
  /* Kill the player if it is touching a wall. */
  if (is_touching_a_wall(game)) {
    player->lives--;
    reposition_player(game);
    /* Unset physics collisions for the player. */
    player->physics = 0;
    player->speed_x = 0;
    player->can_double_jump = 0;
    player->remaining_jump_height = 0;
  }
}

static int can_move_up(const Game *game) {
  const int x = game->player->x;
  const int y = game->player->y;
  if (y == game->box->min_y) {
    return 1;
  }
  return !get_from_rigid_matrix(game, x, y - 1);
}

/**
 * Updates the vertical position of the player.
 */
void update_player_vertical_position(Game *game) {
  const int jumping_speed = PLAYER_JUMPING_SPEED * game->tile_h;
  const int falling_speed = PLAYER_FALLING_SPEED * game->tile_h;
  int pending;
  if (is_jumping(game->player)) {
    if (can_move_up(game)) {
      pending = get_pending_movement(game, jumping_speed);
      while (pending > 0) {
        move_player(game, 0, -1);
        game->player->remaining_jump_height--;
        pending--;
      }
    } else {
      game->player->remaining_jump_height = 0;
    }
  } else if (is_falling(game)) {
    if (game->player->perk == PERK_POWER_LOW_GRAVITY) {
      pending = get_pending_movement(game, falling_speed / 2);
    } else {
      pending = get_pending_movement(game, falling_speed);
    }
    while (pending > 0) {
      move_player(game, 0, 1);
      pending--;
    }
  }
}

void update_double_jump(Game *game) {
  if (is_standing_on_platform(game)) {
    game->player->can_double_jump = 1;
  }
}

static void write_got_perk_message(Game *game, const Perk perk) {
  char message[MAXIMUM_STRING_SIZE];
  sprintf(message, "Got %s!", get_perk_name(perk));
  game_set_message(game, message, 1, 0);
}

static void write_perk_faded_message(Game *game, const Perk perk) {
  char message[MAXIMUM_STRING_SIZE];
  sprintf(message, "%s has faded.", get_perk_name(perk));
  game_set_message(game, message, 1, 0);
}

static void write_perk_fading_message(Game *game, const Perk perk, const unsigned long remaining_frames) {
  const int seconds = remaining_frames / FPS;
  char message[MAXIMUM_STRING_SIZE];
  const char *perk_name = get_perk_name(perk);
  if (seconds < 1) {
    sprintf(message, "%s will fade at any moment.", perk_name);
  } else if (seconds == 1) {
    sprintf(message, "%s will fade in %d second.", perk_name, seconds);
  } else {
    sprintf(message, "%s will fade in %d seconds.", perk_name, seconds);
  }
  game_set_message(game, message, 1, 0);
}

static int is_touching_perk(const Game *const game) {
  const Player *player = game->player;
  const BoundingBox player_box = derive_box(game, player->x, player->y);
  const BoundingBox perk_box = derive_box(game, game->perk_x, game->perk_y);
  return bounding_box_overlaps(&player_box, &perk_box);
}

static void update_player_perk(Game *game) {
  unsigned long end_frame;
  unsigned long remaining_frames;
  Player *player = game->player;
  Perk perk = PERK_NONE;
  if (player->physics) {
    game->played_frames++;
    /* Check for expiration of the player's perk. */
    if (player->perk != PERK_NONE) {
      remaining_frames = player->perk_end_frame - game->played_frames;
      if (remaining_frames == 0) {
        write_perk_faded_message(game, player->perk);
        player->perk = PERK_NONE;
      } else if (remaining_frames <= MINIMUM_REMAINING_FRAMES_FOR_MESSAGE) {
        write_perk_fading_message(game, player->perk, remaining_frames);
      }
    }
    if (game->perk != PERK_NONE) {
      if (is_touching_perk(game)) {
        /* Copy the Perk to transfer it to the Player */
        perk = game->perk;
        /* Remove the Perk from the screen */
        game->perk = PERK_NONE;
        /* Do not update game->perk_end_frame as it is used to */
        /* calculate when the next perk is going to be created */
        /* Attribute the Perk to the Player */
        player->perk = perk;
        if (is_bonus_perk(perk) || is_curse_perk(perk)) {
          if (is_bonus_perk(perk)) {
            conceive_bonus(player, perk);
          } else {
            process_curse(game, perk);
          }
          /* The perk ended now. */
          player->perk_end_frame = game->played_frames;
          /* Could set it to the next frame so that the check above */
          /* this part would removed it, but this seems more correct. */
          player->perk = PERK_NONE;
        } else {
          end_frame = game->played_frames + PERK_PLAYER_DURATION_IN_FRAMES;
          player->perk_end_frame = end_frame;
        }
        write_got_perk_message(game, perk);
      }
    }
  }
}

static void update_player_graphics(Game *game) {
  const int x = game->player->x;
  const int y = game->player->y;
  graphics_update_trail(game->player->graphics, x, y);
}

void update_player(Game *game, Player *player) {
  profiler_begin("update_player");
  if (player->physics) {
    log_player_score(game->played_frames, player->score);
  }
  update_player_graphics(game);
  update_player_perk(game);
  update_player_investments(game);
  process_command(game, player);
  /* This ordering makes the player run horizontally before falling.
   * This seems to be the expected order from an user point-of-view. */
  update_player_horizontal_position(game);
  /* After moving, if it even happened, simulate jumping and falling. */
  update_player_vertical_position(game);
  /* Enable double jump if the player is standing over a platform. */
  update_double_jump(game);
  check_for_player_death(game);
  profiler_end("update_player");
}
