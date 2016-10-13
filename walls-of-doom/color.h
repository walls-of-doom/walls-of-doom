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

/* These colors work as fallback if settings fails. */
extern Color COLOR_DEFAULT_FOREGROUND;
extern Color COLOR_DEFAULT_BACKGROUND;
extern ColorPair COLOR_PAIR_DEFAULT;
extern ColorPair COLOR_PAIR_PERK;
extern ColorPair COLOR_PAIR_PLAYER;
extern ColorPair COLOR_PAIR_TOP_BAR;
extern ColorPair COLOR_PAIR_PLATFORM;
extern ColorPair COLOR_PAIR_BOTTOM_BAR;

ColorPair color_pair_from_string(const char *string);

int color_equals(Color a, Color b);

int color_pair_equals(ColorPair a, ColorPair b);

SDL_Color to_sdl_color(Color color);
Color color_from_rgb(unsigned char r, unsigned char g, unsigned char b);
ColorPair color_pair_from_colors(Color foreground, Color background);

#endif
