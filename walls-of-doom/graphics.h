#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "point.h"
#include <stdlib.h>

typedef struct Graphics {
  Point *trail;
  size_t trail_size;
} Graphics;

Graphics *create_graphics(size_t trail_size);
Graphics *free_graphics(Graphics *graphics);

#endif
