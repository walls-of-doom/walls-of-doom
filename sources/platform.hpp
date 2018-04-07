#ifndef PLATFORM_H
#define PLATFORM_H

#include "box.hpp"
#include "integers.hpp"
#include <vector>

class Platform {
public:
  int x{};
  int y{};
  int w{};
  int h{};
  int speed{};
  float rarity = 0.0f;

  bool operator==(const Platform &rhs) const;
  bool operator!=(const Platform &rhs) const;
};

std::vector<Platform> generate_platforms(BoundingBox box, BoundingBox avoidance, U64 count, int width, int height);

#endif
