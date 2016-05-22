#ifndef BOX_H
#define BOX_H

#include "vector.h"

typedef struct BoundingBox {
    int min_x;
    int min_y;
    int max_x;
    int max_y;
} BoundingBox;

Vector bounding_box_center(const BoundingBox * const box);

#endif
