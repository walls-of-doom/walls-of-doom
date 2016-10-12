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

#define MINIMUM_WIDTH 4
#define MAXIMUM_WIDTH 16

/* The platform speed bounds, these are multiplied by the base speed. */
#define MINIMUM_SPEED 1
#define MAXIMUM_SPEED 4

void generate_platforms(Platform *platforms, const BoundingBox *box,
                        const int count) {
  Platform *platform;
  const int lines = box->max_y - box->min_y + 1;
  unsigned char *density = NULL;
  int random_y;
  int speed;
  int i;
  density = resize_memory(density, sizeof(unsigned char) * lines);
  memset(density, 0, lines);
  for (i = 0; i < count; i++) {
    platform = platforms + i;
    platform->width = random_integer(MINIMUM_WIDTH, MAXIMUM_WIDTH);
    /* Subtract two to remove the borders. */
    /* Subtract one after this to prevent platform being after the screen. */
    platform->x = random_integer(0, get_columns() - 2 - 1) + box->min_x;
    random_y = select_random_line_awarely(density, lines);
    density[random_y]++;
    platform->y = random_y + box->min_y;
    platform->speed_x = 0;
    platform->speed_y = 0;
    speed = PLATFORM_BASE_SPEED * random_integer(MINIMUM_SPEED, MAXIMUM_SPEED);
    /* Make about half the platforms go left and about half go right. */
    /* Make sure that the position is OK to trigger repositioning. */
    if (random_integer(0, 1)) {
      platform->speed_x = speed;
    } else {
      platform->speed_x = -speed;
    }
  }
  resize_memory(density, 0);
}
