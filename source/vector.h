#ifndef VECTOR_H
#define VECTOR_H

/**
 * A 2D floating-point vector.
 */
typedef struct Vector {
    double x;
    double y;
} Vector;

/**
 * Adds two Vectors and returns the result.
 */
Vector vector_add(const Vector a, const Vector b);

#endif
