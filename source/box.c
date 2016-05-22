#include "box.h"

Vector bounding_box_center(const BoundingBox * const box) {
    Vector center;
    center.x = (box->min_x + box->max_x) / 2;
    center.y = (box->min_y + box->max_y) / 2;
    return center;
}
