#include "color.h"

#include <SDL.h>

const Color BACKGROUND_COLOR = {0x22, 0x22, 0x22, 0xFF};
const ColorPair DEFAULT_COLOR = {{0xBB, 0xBB, 0xBB, 0xFF},
                                 {0x22, 0x22, 0x22, 0xFF}};

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
