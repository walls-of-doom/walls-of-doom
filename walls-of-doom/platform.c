#include "platform.h"
#include "constants.h"
#include "data.h"
#include "logger.h"
#include "memory.h"
#include "physics.h"
#include "random.h"
#include "settings.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

void generate_platforms(Platform *platforms, BoundingBox *box, int count) {
  const int min_width = get_platform_min_width();
  const int max_width = get_platform_max_width();
  const int min_speed = get_platform_min_speed();
  const int max_speed = get_platform_max_speed();
  const int lines = box->max_y - box->min_y + 1;
  unsigned char *density = NULL;
  Platform *platform;
  int random_y;
  int speed;
  int i;
  density = resize_memory(density, sizeof(unsigned char) * lines);
  memset(density, 0, lines);
  for (i = 0; i < count; i++) {
    platform = platforms + i;
    platform->width = random_integer(min_width, max_width);
    /* Subtract two to remove the borders. */
    /* Subtract one after this to prevent platform being after the screen. */
    platform->x = random_integer(0, get_columns() - 2 - 1) + box->min_x;
    random_y = select_random_line_awarely(density, lines);
    density[random_y]++;
    platform->y = random_y + box->min_y;
    platform->speed = 0;
    speed = PLATFORM_BASE_SPEED * random_integer(min_speed, max_speed);
    /* Make about half the platforms go left and about half go right. */
    /* Make sure that the position is OK to trigger repositioning. */
    if (random_integer(0, 1)) {
      platform->speed = speed;
    } else {
      platform->speed = -speed;
    }
  }
  resize_memory(density, 0);
}

/**
 * Compares two Platforms and evaluates whether or not they are the same.
 */
int platform_equals(const Platform a, const Platform b) {
  const int pos_equals = a.x == b.x && a.y == b.y;
  return pos_equals && a.speed == b.speed && a.width == b.width;
}
