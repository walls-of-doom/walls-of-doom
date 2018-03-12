#ifndef PLATFORM_H
#define PLATFORM_H

#include "box.h"

typedef struct Platform {
  int x;
  int y;
  int w;
  int h;
  int speed;
} Platform;

void generate_platforms(Platform *platforms, const BoundingBox *const box, const int count, const int width,
                        const int height);

/**
 * Compares two Platforms and evaluates whether or not they are the same.
 */
int platform_equals(const Platform a, const Platform b);

#endif
