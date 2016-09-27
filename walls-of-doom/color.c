#include "color.h"

#include <SDL.h>

const Color BACKGROUND_COLOR = {0x22, 0x22, 0x22, 0xFF};
const ColorPair DEFAULT_COLOR = {{0xBB, 0xBB, 0xBB, 0xFF},
                                 {0x22, 0x22, 0x22, 0xFF}};
const ColorPair PLATFORM_COLOR = {{0xBB, 0xBB, 0xBB, 0xFF},
                                  {0xAA, 0xAA, 0xAA, 0xFF}};
/* Dark gray with pastel red. */
const ColorPair TOP_BAR_COLOR = {{0x22, 0x22, 0x22, 0xFF},
                                 {0xFF, 0x69, 0x61, 0xFF}};
/* Dark gray with blue-gray. */
const ColorPair BOTTOM_BAR_COLOR = {{0xBB, 0xBB, 0xBB, 0xFF},
                                    {0x66, 0x99, 0xCC, 0xFF}};

int color_equals(Color a, Color b) {
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

int color_pair_equals(ColorPair a, ColorPair b) {
  const int fore_equals = color_equals(a.foreground, b.foreground);
  const int back_equals = color_equals(a.background, b.background);
  return fore_equals && back_equals;
}

Color color_from_rgb(uint8_t r, uint8_t g, uint8_t b) {
  Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = 0xFF;
  return color;
}

SDL_Color to_sdl_color(Color color) {
  SDL_Color sdl_color;
  sdl_color.r = color.r;
  sdl_color.g = color.g;
  sdl_color.b = color.b;
  sdl_color.a = color.a;
  return sdl_color;
}

ColorPair color_pair_from_colors(Color foreground, Color background) {
  ColorPair pair;
  pair.foreground = foreground;
  pair.background = background;
  return pair;
}
