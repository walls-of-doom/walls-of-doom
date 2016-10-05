#ifndef BOX_H
#define BOX_H

typedef struct BoundingBox {
  int min_x;
  int min_y;
  int max_x;
  int max_y;
} BoundingBox;

/**
 * Compares two BoundingBox objects for equality.
 */
int bounding_box_equals(const BoundingBox *a, const BoundingBox *const b);

#endif
