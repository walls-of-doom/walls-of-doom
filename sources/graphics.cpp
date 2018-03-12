#include "graphics.h"
#include "memory.h"
#include "point.h"

/* Appends (x, y) to the trail. */
void graphics_update_trail(Graphics *graphics, int x, int y) {
  const size_t head = graphics->trail_head;
  const size_t size = graphics->trail_size;
  const size_t index = (head + size) % graphics->trail_capacity;
  graphics->trail[index].x = x;
  graphics->trail[index].y = y;
  if (graphics->trail_size < graphics->trail_capacity) {
    graphics->trail_size++;
  } else {
    graphics->trail_head = (head + 1) % graphics->trail_capacity;
  }
}

Graphics *create_graphics(size_t trail_size) {
  Graphics *graphics = reinterpret_cast<Graphics *>(resize_memory(NULL, sizeof(Graphics)));
  graphics->trail = reinterpret_cast<Point *>(resize_memory(NULL, trail_size * sizeof(Point)));
  graphics->trail_head = 0;
  graphics->trail_size = 0;
  graphics->trail_capacity = trail_size;
  return graphics;
}

Graphics *destroy_graphics(Graphics *graphics) {
  /* Free the trail before the Graphics object. */
  graphics->trail = reinterpret_cast<Point *>(resize_memory(graphics->trail, 0));
  return reinterpret_cast<Graphics *>(resize_memory(graphics, 0));
}
