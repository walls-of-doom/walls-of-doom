#include "platform.h"

#include "constants.h"
#include "data.h"
#include "logger.h"
#include "random.h"

#include <stdlib.h>

#define MINIMUM_WIDTH 4
#define MAXIMUM_WIDTH 16

/* The platform speed bounds, these are multiplied by the base speed. */
#define MINIMUM_SPEED 1
#define MAXIMUM_SPEED 4

void generate_platforms(Platform *platforms, int count) {
  int speed;
  int i;
  for (i = 0; i < count; i++) {
    Platform *platform = platforms + i;
    platform->width = random_integer(MINIMUM_WIDTH, MAXIMUM_WIDTH);
    platform->x = random_integer(1, COLUMNS - 1);
    platform->y = random_integer(4, LINES - 4);
    platform->speed_x = 0;
    platform->speed_y = 0;
    speed = PLATFORM_BASE_SPEED * random_integer(MINIMUM_SPEED, MAXIMUM_SPEED);
    /* Make about half the platforms go left and about half go right. */
    if (random_integer(0, 1)) {
      platform->speed_x = speed;
    } else {
      platform->speed_x = -speed;
    }
  }
}
