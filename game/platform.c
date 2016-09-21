#include "platform.h"

#include "constants.h"
#include "data.h"
#include "logger.h"
#include "random.h"

#include <stdlib.h>

/**
 * How many integers the array must have.
 *
 * 1 for the platform count and 2 for each platform.
 */
#define READ_PLATFORMS_INTEGER_ARRAY_SIZE 1 + 2 * MAXIMUM_PLATFORM_COUNT

/**
 * Reads platforms from the text file into the provided buffer.
 *
 * Returning the number of platforms read.
 *
 * This number is guaranteed to be less than MAXIMUM_PLATFORM_COUNT.
 */
int read_platforms(Platform *platforms) {
  int input_integers[READ_PLATFORMS_INTEGER_ARRAY_SIZE];
  size_t actually_read;
  size_t platform_count;
  char log_message_buffer[256];
  size_t i;
  int speed;
  int movement_type;

  log_message("Started reading platform data");
  actually_read = read_integers("assets/platforms.txt", input_integers,
                                READ_PLATFORMS_INTEGER_ARRAY_SIZE);

  sprintf(log_message_buffer, "Read %lu integers", actually_read);
  log_message(log_message_buffer);

  if (actually_read > 0) {
    platform_count = input_integers[0] < MAXIMUM_PLATFORM_COUNT
                         ? input_integers[0]
                         : MAXIMUM_PLATFORM_COUNT;
  } else {
    platform_count = 0;
  }
  sprintf(log_message_buffer, "Platform count is %lu", platform_count);
  log_message(log_message_buffer);

  for (i = 0; i < platform_count; i++) {
    Platform *platform = platforms + i;

    platform->width = input_integers[1 + 2 * i];

    platform->x = random_integer(1, COLUMNS - 1);
    platform->y = random_integer(4, LINES - 4);

    platform->speed_x = 0;
    platform->speed_y = 0;
    speed = input_integers[1 + 2 * i + 1] * PLATFORM_BASE_SPEED;
    movement_type = random_integer(0, 4);
    if (movement_type < 2) { /* 40% */
      platform->speed_x = speed;
    } else if (movement_type < 4) { /* 40% */
      platform->speed_x = -speed;
    } else { /* 20% */
      platform->speed_y = -speed;
    }
  }

  return platform_count;
}
