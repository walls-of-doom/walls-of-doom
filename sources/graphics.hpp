#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "point.hpp"
#include <stdlib.h>

typedef struct Graphics {
  Point *trail;
  size_t trail_head;
  size_t trail_size;
  size_t trail_capacity;
} Graphics;

/* Appends (x, y) to the trail. */
void graphics_update_trail(Graphics *graphics, int x, int y);

Graphics *create_graphics(size_t trail_size);
Graphics *destroy_graphics(Graphics *graphics);

#endif
