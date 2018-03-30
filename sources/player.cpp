#include "player.hpp"
#include "command.hpp"
#include "graphics.hpp"
#include "logger.hpp"
#include <climits>
#include <cstdlib>

#define MAXIMUM_PLAYER_SCORE LONG_MAX
#define MINIMUM_PLAYER_SCORE 0

#define DEFAULT_TRAIL_SIZE 4

Player::Player(std::string name, CommandTable *table) : name(std::move(name)), table(table) {
  x = 0;
  y = 0;
  w = 0;
  h = 0;
  speed_x = 0;
  speed_y = 0;
  physics = false;
  can_double_jump = 0;
  remaining_jump_height = 0;
  lives = 3;
  score = 0;
  perk = PERK_NONE;
  perk_end_frame = 0;
  investments = nullptr;
  graphics = create_graphics(DEFAULT_TRAIL_SIZE);
}

Player::~Player() { destroy_graphics(graphics); }

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
