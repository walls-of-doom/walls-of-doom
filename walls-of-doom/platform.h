#ifndef PLATFORM_H
#define PLATFORM_H

#include "box.h"

typedef struct Platform {
  int x;
  int y;
  int speed_x;
  int speed_y;
  int width;
} Platform;

void generate_platforms(Platform *platforms, const BoundingBox *box,
                        const int count);

#endif
