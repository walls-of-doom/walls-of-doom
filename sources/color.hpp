#ifndef COLOR_H
#define COLOR_H

#include "integers.hpp"
#include <SDL.h>
#include <string>

class Color {
public:
  U8 r = 0;
  U8 g = 0;
  U8 b = 0;
  U8 a = 0;

  Color() = default;
  Color(U8 r, U8 g, U8 b) : r(r), g(g), b(b) {}
  Color(U8 r, U8 g, U8 b, U8 a) : r(r), g(g), b(b), a(a) {}

  inline Color mix(Color rhs, float rhsAmount) const {
    const auto lhsAmount = 1.0f - rhsAmount;
    const auto endR = static_cast<U8>(r * lhsAmount + rhs.r * rhsAmount);
    const auto endG = static_cast<U8>(g * lhsAmount + rhs.g * rhsAmount);
    const auto endB = static_cast<U8>(b * lhsAmount + rhs.b * rhsAmount);
    const auto endA = static_cast<U8>(a * lhsAmount + rhs.a * rhsAmount);
    return Color(endR, endG, endB, endA);
  }

  inline SDL_Color to_SDL_color() const {
    SDL_Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
  }
};

class ColorPair {
public:
  Color foreground;
  Color background;
};

U8 parse_base16_digit_pair(std::string string);

ColorPair color_pair_from_string(const std::string &);

/* These colors work as fallback if settings fails. */
extern Color COLOR_DEFAULT_FOREGROUND;
extern Color COLOR_DEFAULT_BACKGROUND;

extern ColorPair COLOR_PAIR_DEFAULT;

extern ColorPair COLOR_PAIR_PERK;
extern ColorPair COLOR_PAIR_PLAYER;

extern ColorPair COLOR_PAIR_TOP_BAR;
extern ColorPair COLOR_PAIR_BOTTOM_BAR;

extern ColorPair COLOR_PAIR_PLATFORM_A;
extern ColorPair COLOR_PAIR_PLATFORM_B;

#endif
