#include "player.hpp"
#include "logger.hpp"

static const Score MAXIMUM_PLAYER_SCORE = std::numeric_limits<Score>::max();
static const Score MINIMUM_PLAYER_SCORE = 0;
static const int DEFAULT_TRAIL_SIZE = 4;

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
}

void Player::decrement_score(const Score amount) {
  const Score maximum_sub = score - MINIMUM_PLAYER_SCORE;
  if (maximum_sub >= amount) {
    score -= amount;
  } else {
    log_message("Prevented Player score underflow!");
    score = MINIMUM_PLAYER_SCORE;
  }
}

void Player::increment_score(const Score amount) {
  const Score maximum_add = MAXIMUM_PLAYER_SCORE - score;
  if (maximum_add >= amount) {
    score += amount;
  } else {
    log_message("Prevented Player score overflow!");
    score = MAXIMUM_PLAYER_SCORE;
  }
}

void Player::increment_score_from_event(const float rarity) { score += 100.0f / UPS * (1.0f + rarity); }
