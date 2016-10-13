#ifndef COLOR_H
#define COLOR_H

#include <SDL.h>

typedef struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Color;

typedef struct ColorPair {
  Color foreground;
  Color background;
} ColorPair;

extern const Color BACKGROUND_COLOR;
extern const ColorPair DEFAULT_COLOR;
extern const ColorPair PERK_COLOR;
extern const ColorPair TOP_BAR_COLOR;
extern const ColorPair BOTTOM_BAR_COLOR;
extern const ColorPair PLATFORM_COLOR;
extern const ColorPair PLAYER_COLOR;

int color_equals(Color a, Color b);

int color_pair_equals(ColorPair a, ColorPair b);

SDL_Color to_sdl_color(Color color);
Color color_from_rgb(unsigned char r, unsigned char g, unsigned char b);
ColorPair color_pair_from_colors(Color foreground, Color background);

#endif
