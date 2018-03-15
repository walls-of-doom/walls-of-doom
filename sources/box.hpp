#ifndef BOX_H
#define BOX_H

class BoundingBox {
public:
  int min_x = 0;
  int min_y = 0;
  int max_x = 0;
  int max_y = 0;

  bool contains(int x, int y) const;

  /**
   * Checks for overlap of two BoundingBox objects.
   */
  bool overlaps(const BoundingBox &rhs) const;

  bool operator==(const BoundingBox &rhs) const;
  bool operator!=(const BoundingBox &rhs) const;
};

int bounding_box_width(const BoundingBox *a);

#endif
