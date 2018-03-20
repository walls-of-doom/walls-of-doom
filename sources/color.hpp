#ifndef COLOR_H
#define COLOR_H

#include "integers.hpp"
#include <SDL.h>

class Color {
public:
  U8 r = 0;
  U8 g = 0;
  U8 b = 0;
  U8 a = 0;

  Color mix(Color rhs, float rhsAmount) {
    const auto lhsAmount = 1.0f - rhsAmount;
    const auto endR = static_cast<U8>(r * lhsAmount + rhs.r * rhsAmount);
    const auto endG = static_cast<U8>(g * lhsAmount + rhs.g * rhsAmount);
    const auto endB = static_cast<U8>(b * lhsAmount + rhs.b * rhsAmount);
    const auto endA = static_cast<U8>(a * lhsAmount + rhs.a * rhsAmount);
    return {endR, endG, endB, endA};
  }
};

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
extern ColorPair COLOR_PAIR_BOTTOM_BAR;

extern ColorPair COLOR_PAIR_PLATFORM;
extern ColorPair COLOR_PAIR_PLATFORM_RARE;

ColorPair color_pair_from_string(const char *string);

int color_equals(Color a, Color b);

int color_pair_equals(ColorPair a, ColorPair b);

SDL_Color to_sdl_color(Color color);
Color color_from_rgb(unsigned char r, unsigned char g, unsigned char b);
Color mix_colors(Color a, Color b);
ColorPair color_pair_from_colors(Color foreground, Color background);

#endif
