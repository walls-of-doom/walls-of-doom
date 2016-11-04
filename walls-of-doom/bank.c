#include "bank.h"
#include "game.h"
#include "investment.h"
#include "random.h"
#include "settings.h"

static double get_average_width(Game const *const game) {
  double total = 0;
  size_t i;
  for (i = 0; i != game->platform_count; ++i) {
    total += game->platforms[i].width;
  }
  return total / (double)game->platform_count;
}

static double get_average_speed(Game const *const game) {
  double total = 0;
  size_t i;
  for (i = 0; i != game->platform_count; ++i) {
    total += game->platforms[i].speed;
  }
  return total / (double)game->platform_count;
}

/**
 * Returns a normalized value (from 0 to 1) indicating the game difficulty.
 */
static double get_difficulty(Game const *const game) {
  const int min_width = get_platform_min_width();
  const int max_width = get_platform_max_width();
  const int min_speed = get_platform_min_speed();
  const int max_speed = get_platform_max_speed();
  const double avg_width = (min_width + max_width) / 2;
  const double avg_speed = (min_speed + max_speed) / 2;
  const double game_avg_width = get_average_width(game);
  const double game_avg_speed = get_average_speed(game);
  /* Wider platforms make the game easier. */
  const double width_ratio = avg_width / game_avg_width;
  /* Faster platforms make the game harder. */
  const double speed_ratio = game_avg_speed / avg_speed;
  return width_ratio * speed_ratio;
}

int collect_investment(Game const *const game, const Investment investment) {
  const int max_return = get_investment_maximum_factor();
  const int min_return = get_investment_minimum_factor();
  const int difference = max_return - min_return;
  const int normalized_delta = (int)(get_difficulty(game) * difference);
  const int normalized_max_return = min_return + normalized_delta;
  const int random_factor = random_integer(min_return, normalized_max_return);
  return random_factor * investment.amount / 100;
}
