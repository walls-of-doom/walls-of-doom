#ifndef PLATFORM_H
#define PLATFORM_H

#include "box.h"

typedef struct Platform {
  int x;
  int y;
  int speed;
  int width;
} Platform;

void generate_platforms(Platform *platforms, BoundingBox *box, int count);

/**
 * Compares two Platforms and evaluates whether or not they are the same.
 */
int platform_equals(const Platform a, const Platform b);

#endif
