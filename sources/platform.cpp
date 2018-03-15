#include "platform.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "physics.hpp"
#include "random.hpp"
#include "settings.hpp"
#include <climits>
#include <cstdlib>
#include <cstring>

void generate_platforms(Platform *platforms, const BoundingBox *const box, const int count, const int width,
                        const int height) {
  const int min_width = get_platform_min_width() * width;
  const int max_width = get_platform_max_width() * width;
  const int min_speed = get_platform_min_speed();
  const int max_speed = get_platform_max_speed();
  const int lines = (box->max_y - box->min_y + 1) / height;
  unsigned char *density = nullptr;
  Platform *platform;
  int random_y;
  int speed;
  int i;
  density = reinterpret_cast<unsigned char *>(resize_memory(density, sizeof(unsigned char) * lines));
  memset(density, 0, lines);
  for (i = 0; i < count; i++) {
    platform = platforms + i;
    platform->h = height;
    platform->w = random_integer(min_width, max_width);
    /* Subtract two to remove the borders. */
    /* Subtract one after this to prevent platform being after the screen. */
    platform->x = random_integer(0, bounding_box_width(box)) + box->min_x;
    random_y = select_random_line_awarely(density, lines);
    density[random_y]++;
    platform->y = random_y * height + box->min_y;
    platform->speed = 0;
    speed = random_integer(min_speed, max_speed);
    /* Make about half the platforms go left and about half go right. */
    /* Make sure that the position is OK to trigger repositioning. */
    if (random_integer(0, 1) != 0) {
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
  const auto position_equals = static_cast<const int>(a.x == b.x && a.y == b.y);
  const auto speed_equals = static_cast<const int>(a.speed == b.speed);
  const auto size_equals = static_cast<const int>(a.w == b.w && a.h == b.h);
  return static_cast<int>((position_equals != 0) && (speed_equals != 0) && (size_equals) != 0);
}
