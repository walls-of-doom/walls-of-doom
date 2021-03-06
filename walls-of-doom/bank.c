#include "bank.h"
#include "game.h"
#include "investment.h"
#include "logger.h"
#include "random.h"
#include "score.h"
#include "settings.h"
#include <stdlib.h>

static double get_average_width(Game const *const game) {
  double total = 0.0;
  size_t i;
  for (i = game->platform_count - 1; i; --i) {
    total += game->platforms[i].w;
  }
  return total / (double)game->platform_count;
}

static double get_average_speed(Game const *const game) {
  double total = 0.0;
  size_t i;
  for (i = game->platform_count - 1; i; --i) {
    total += abs(game->platforms[i].speed);
  }
  return total / (double)game->platform_count;
}

static void log_difficulty(const double difficulty) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  sprintf(log_buffer, "Difficulty is %f.", difficulty);
  log_message(log_buffer);
}

/**
 * Returns a normalized value (from 0 to 1) indicating the game difficulty.
 */
static double get_difficulty(Game const *const game) {
  const int min_width = get_platform_min_width() * game->tile_w;
  const int max_width = get_platform_max_width() * game->tile_w;
  const int min_speed = get_platform_min_speed() * game->tile_w;
  const int max_speed = get_platform_max_speed() * game->tile_w;
  const double avg_width = (min_width + max_width) / 2.0;
  const double avg_speed = (min_speed + max_speed) / 2.0;
  const double game_avg_width = get_average_width(game);
  const double game_avg_speed = get_average_speed(game);
  /* Wider platforms make the game easier. */
  const double width_ratio = avg_width / game_avg_width;
  /* Faster platforms make the game harder. */
  const double speed_ratio = game_avg_speed / avg_speed;
  const double difficulty = width_ratio * speed_ratio;
  /* Log the difficulty coefficient once. */
  static int logged_difficulty = 0;
  if (!logged_difficulty) {
    log_difficulty(difficulty);
    logged_difficulty = 1;
  }
  return difficulty;
}

int collect_investment(Game const *const game, const Investment investment) {
  /* We generate random integers instead of doubles. */
  /* Therefore we have to scale the double up then divide by an integer. */
  const int scaling_factor = 1000;
  const double max_return = get_investment_maximum_factor();
  const double min_return = get_investment_minimum_factor();
  const double difference = max_return - min_return;
  const double normalized_delta = get_difficulty(game) * difference;
  const double normalized_max_return = min_return + normalized_delta;
  const int min_random = (int)(scaling_factor * min_return);
  const int max_random = (int)(scaling_factor * normalized_max_return);
  const double random_factor = random_integer(min_random, max_random);
  return (int)(random_factor * investment.amount / scaling_factor);
}
