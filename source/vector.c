#include "vector.h"

/**
 * Adds two Vectors and returns the result.
 */
Vector vector_add(const Vector a, const Vector b) {
    Vector result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}
