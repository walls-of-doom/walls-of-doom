#ifndef BOX_H
#define BOX_H

typedef struct BoundingBox {
  int min_x;
  int min_y;
  int max_x;
  int max_y;
} BoundingBox;

int bounding_box_contains(const BoundingBox *box, const int x, const int y);

/**
 * Compares two BoundingBox objects for equality.
 */
int bounding_box_equals(const BoundingBox *a, const BoundingBox *const b);

/**
 * Checks for overlap of two BoundingBox objects.
 */
int bounding_box_overlaps(const BoundingBox *a, const BoundingBox *const b);

int bounding_box_width(const BoundingBox *a);

long bounding_box_area(const BoundingBox *box);

#endif
