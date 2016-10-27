#include "color.h"
#include <SDL.h>

#define COLOR_STRING_SEPARATOR ','

/* These colors work as fallback if settings fails. */
Color COLOR_DEFAULT_FOREGROUND = {192, 192, 192, 255};
Color COLOR_DEFAULT_BACKGROUND = {32, 32, 32, 255};
ColorPair COLOR_PAIR_DEFAULT = {{192, 192, 192, 255}, {32, 32, 32, 255}};
ColorPair COLOR_PAIR_PERK = {{192, 192, 192, 255}, {32, 32, 32, 255}};
ColorPair COLOR_PAIR_PLAYER = {{192, 192, 192, 255}, {32, 32, 32, 255}};
ColorPair COLOR_PAIR_TOP_BAR = {{192, 192, 192, 255}, {32, 32, 32, 255}};
ColorPair COLOR_PAIR_PLATFORM = {{192, 192, 192, 255}, {32, 32, 32, 255}};
ColorPair COLOR_PAIR_BOTTOM_BAR = {{192, 192, 192, 255}, {32, 32, 32, 255}};

static unsigned char parse_two_hexadecimal_characters(const char *string) {
  char substring[3];
  substring[0] = string[0];
  substring[1] = string[1];
  substring[2] = '\0';
  return (unsigned char)strtoul(substring, NULL, 16);
}

Color color_from_string(const char *string) {
  Color color;
  color.r = parse_two_hexadecimal_characters(string);
  color.g = parse_two_hexadecimal_characters(string + 2);
  color.b = parse_two_hexadecimal_characters(string + 4);
  color.a = parse_two_hexadecimal_characters(string + 6);
  return color;
}

ColorPair color_pair_from_string(const char *string) {
  ColorPair pair;
  char *const end = strchr(string, COLOR_STRING_SEPARATOR);
  *end = '\0';
  pair.foreground = color_from_string(string);
  *end = COLOR_STRING_SEPARATOR;
  pair.background = color_from_string(end + 1);
  return pair;
}

int color_equals(Color a, Color b) {
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

int color_pair_equals(ColorPair a, ColorPair b) {
  const int fore_equals = color_equals(a.foreground, b.foreground);
  const int back_equals = color_equals(a.background, b.background);
  return fore_equals && back_equals;
}

Color color_from_rgb(unsigned char r, unsigned char g, unsigned char b) {
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
