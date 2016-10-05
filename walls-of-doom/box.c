#include "box.h"

/**
 * Compares two BoundingBox objects for equality.
 */
int bounding_box_equals(const BoundingBox *a, const BoundingBox *const b) {
  const int min_equals = a->min_x == b->min_x && a->min_y == b->min_y;
  const int max_equals = a->max_x == b->max_x && a->max_y == b->max_y;
  return min_equals && max_equals;
}
