#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <list>

#include "point.hpp"

class Graphics {
public:
  inline explicit Graphics(size_t maximum_size) : maximum_size(maximum_size) {}

  inline void update_trail(S32 x, S32 y) {
    trail.emplace_back(x, y);
    if (trail.size() > maximum_size) {
      trail.pop_front();
    }
  }

  inline size_t get_maximum_size() const { return maximum_size; }

  std::list<Point> trail;

private:
  size_t maximum_size;
};

#endif
