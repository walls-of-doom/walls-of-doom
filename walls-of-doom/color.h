#ifndef COLOR_H
#define COLOR_H

#include <SDL.h>

#include <stdint.h>

typedef struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} Color;

typedef struct ColorPair {
  Color foreground;
  Color background;
} ColorPair;

extern const Color BACKGROUND_COLOR;
extern const ColorPair DEFAULT_COLOR;
extern const ColorPair TOP_BAR_COLOR;
extern const ColorPair BOTTOM_BAR_COLOR;

SDL_Color to_sdl_color(Color color);
Color color_from_rgb(uint8_t r, uint8_t g, uint8_t b);
ColorPair color_pair_from_colors(Color foreground, Color background);

#endif
