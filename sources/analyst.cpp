#include "analyst.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "random.hpp"
#include "score.hpp"
#include "settings.hpp"
#include <cstdlib>

static F64 get_average_width(const Game &game) {
  F64 total = 0.0;
  for (size_t i = game.platform_count - 1; i != 0u; --i) {
    total += game.platforms[i].w;
  }
  return total / static_cast<F64>(game.platform_count);
}

static F64 get_average_speed(const Game &game) {
  F64 total = 0.0;
  for (size_t i = game.platform_count - 1; i != 0u; --i) {
    total += abs(game.platforms[i].speed);
  }
  return total / static_cast<F64>(game.platform_count);
}

F64 get_difficulty(const Game &game) {
  const S32 min_width = game.settings->get_platform_min_width() * game.tile_w;
  const S32 max_width = game.settings->get_platform_max_width() * game.tile_w;
  const S32 min_speed = game.settings->get_platform_min_speed() * game.tile_w;
  const S32 max_speed = game.settings->get_platform_max_speed() * game.tile_w;
  const F64 avg_width = (min_width + max_width) / 2.0;
  const F64 avg_speed = (min_speed + max_speed) / 2.0;
  const F64 game_avg_width = get_average_width(game);
  const F64 game_avg_speed = get_average_speed(game);
  /* Wider platforms make the game easier. */
  const F64 width_ratio = avg_width / game_avg_width;
  /* Faster platforms make the game harder. */
  const F64 speed_ratio = game_avg_speed / avg_speed;
  const F64 difficulty = width_ratio * speed_ratio;
  return difficulty;
}
