#include "box.hpp"

bool BoundingBox::operator==(const BoundingBox &rhs) const {
  return min_x == rhs.min_x && min_y == rhs.min_y && max_x == rhs.max_x && max_y == rhs.max_y;
}

bool BoundingBox::operator!=(const BoundingBox &rhs) const { return !(rhs == *this); }

bool BoundingBox::contains(int x, int y) const { return (x >= min_x && x <= max_x) && (y >= min_y && y <= max_y); }

bool BoundingBox::overlaps(const BoundingBox &rhs) const {
  /* Two boxes overlap if, and only if, a corner is contained in another. */
  if (contains(rhs.min_x, rhs.min_y)) {
    return true;
  }
  if (contains(rhs.min_x, rhs.max_y)) {
    return true;
  }
  if (contains(rhs.max_x, rhs.min_y)) {
    return true;
  }
  return contains(rhs.max_x, rhs.max_y);
}

int bounding_box_width(const BoundingBox *a) { return a->max_x - a->min_x; }
