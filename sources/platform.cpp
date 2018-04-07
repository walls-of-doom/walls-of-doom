#include "platform.hpp"
#include "data.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "physics.hpp"
#include <cstring>

std::vector<Platform> generate_platforms(BoundingBox box, U64 count, S32 width, S32 height) {
  const S32 min_width = get_platform_min_width() * width;
  const S32 max_width = get_platform_max_width() * width;
  const S32 min_speed = get_platform_min_speed();
  const S32 max_speed = get_platform_max_speed();
  const auto lines = static_cast<U32>((box.max_y - box.min_y + 1) / height);
  std::vector<U8> density(lines);
  std::vector<Platform> platforms;
  for (U64 i = 0; i < count; i++) {
    platforms.emplace_back();
    Platform &platform = platforms.back();
    platform.h = height;
    platform.w = random_integer(min_width, max_width);
    /* Subtract two to remove the borders. */
    /* Subtract one after this to prevent platform being after the screen. */
    platform.x = random_integer(0, bounding_box_width(&box)) + box.min_x;
    const auto random_y = select_random_line_awarely(density);
    density[random_y]++;
    platform.y = random_y * height + box.min_y;
    platform.speed = 0;
    const auto speed = random_integer(min_speed, max_speed);
    /* Make about half the platforms go left and about half go right. */
    /* Make sure that the position is OK to trigger repositioning. */
    if (random_integer(0, 1) != 0) {
      platform.speed = speed;
    } else {
      platform.speed = -speed;
    }
    platform.rarity = random_integer(0, 4) / 4.0f;
  }
  return platforms;
}

bool Platform::operator==(const Platform &rhs) const {
  return x == rhs.x && y == rhs.y && w == rhs.w && h == rhs.h && speed == rhs.speed && rarity == rhs.rarity;
}

bool Platform::operator!=(const Platform &rhs) const { return !(rhs == *this); }
