#include "graphics.h"
#include "memory.h"
#include "point.h"

Graphics *create_graphics(size_t trail_size) {
  Graphics *graphics = NULL;
  resize_memory(graphics, trail_size * sizeof(Point));
  return graphics;
}

Graphics *free_graphics(Graphics *graphics) {
  return resize_memory(graphics, 0);
}
