#include "high-io.h"
#include "base-io.h"
#include "clock.h"
#include "constants.h"
#include "game.h"
#include "logger.h"
#include "memory.h"
#include "numeric.h"
#include "physics.h"
#include "player.h"
#include "profiler.h"
#include "random.h"
#include "record.h"
#include "settings.h"
#include "text.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define GAME_NAME "Walls of Doom"

#define PERK_FADING_INTERVAL FPS

/**
 * Evaluates whether or not a Player name is a valid name.
 *
 * A name is considered to be valid if it has at least two characters after
 * being trimmed.
 */
static int is_valid_player_name(const char *player_name) {
  char buffer[MAXIMUM_PLAYER_NAME_SIZE];
  copy_string(buffer, player_name, MAXIMUM_PLAYER_NAME_SIZE);
  trim_string(buffer);
  return strlen(buffer) >= 2;
}

/**
 * Attempts to read a player name.
 *
 * Returns a Code, which may indicate that the player tried to quit.
 */
Code read_player_name(char *destination, const size_t maximum_size,
                      Renderer *renderer) {
  int x;
  int y;
  Code code = CODE_ERROR;
  int valid_name = 0;
  const char message[] = "Name your character: ";
  char log_buffer[MAXIMUM_STRING_SIZE];
  random_name(destination);
  /* While there is not a read error or a valid name. */
  while (code != CODE_OK || !valid_name) {
    x = get_padding() * get_font_width();
    y = (get_window_height() - get_font_height()) / 2;
    code = read_string(x, y, message, destination, maximum_size, renderer);
    if (code == CODE_QUIT) {
      return CODE_QUIT;
    } else if (code == CODE_OK) {
      sprintf(log_buffer, "Read '%s' from the user", destination);
      log_message(log_buffer);
      /* Trim the name the user entered. */
      trim_string(destination);
      sprintf(log_buffer, "Trimmed the input to '%s'", destination);
      log_message(log_buffer);
      valid_name = is_valid_player_name(destination);
    }
  }
  return code;
}

static SDL_Rect make_top_bar_rectangle(void) {
  SDL_Rect rectangle;
  rectangle.x = 0;
  rectangle.y = get_bar_height();
  rectangle.w = get_window_width();
  rectangle.h = get_border_height();
  return rectangle;
}

static SDL_Rect make_left_bar_rectangle(void) {
  SDL_Rect rectangle;
  rectangle.x = 0;
  rectangle.y = get_bar_height();
  rectangle.w = get_border_width();
  rectangle.h = get_window_height() - 2 * get_bar_height();
  return rectangle;
}

static SDL_Rect make_right_bar_rectangle(void) {
  SDL_Rect rectangle;
  rectangle.x = get_window_width() - get_border_width();
  rectangle.y = get_bar_height();
  rectangle.w = get_border_width();
  rectangle.h = get_window_height() - 2 * get_bar_height();
  return rectangle;
}

static SDL_Rect make_bottom_bar_rectangle(void) {
  SDL_Rect rectangle;
  rectangle.x = 0;
  rectangle.y = get_window_height() - get_bar_height() - get_border_height();
  rectangle.w = get_window_width();
  rectangle.h = get_border_height();
  return rectangle;
}

static Code render_borders(Renderer *renderer) {
  const SDL_Rect top_bar = make_top_bar_rectangle();
  const SDL_Rect left_bar = make_left_bar_rectangle();
  const SDL_Rect right_bar = make_right_bar_rectangle();
  const SDL_Rect bottom_bar = make_bottom_bar_rectangle();
  SDL_Color color = to_sdl_color(COLOR_DEFAULT_FOREGROUND);
  swap_color(renderer, &color);
  SDL_RenderFillRect(renderer, &top_bar);
  SDL_RenderFillRect(renderer, &left_bar);
  SDL_RenderFillRect(renderer, &right_bar);
  SDL_RenderFillRect(renderer, &bottom_bar);
  swap_color(renderer, &color);
  return CODE_OK;
}

/**
 * Prints the provided string on the screen starting at (x, y).
 */
Code print(const int x, const int y, const char *string,
           const ColorPair color_pair, Renderer *renderer) {
  const int absolute_x = get_tile_width() * x;
  const int absolute_y = get_bar_height() + get_tile_height() * (y - 1);
  return print_absolute(absolute_x, absolute_y, string, color_pair, renderer);
}

/**
 * Replaces the first line break of any sequence of line breaks by a space.
 */
void print_menu(int line_count, char **lines, Renderer *renderer) {
  clear(renderer);
  print_centered_vertically(line_count, lines, COLOR_PAIR_DEFAULT, renderer);
  present(renderer);
}

/**
 * Draws an absolute rectangle based on the provided coordinates.
 */
static void draw_absolute_rectangle(const int x, const int y, const int w,
                                    const int h, Color color,
                                    Renderer *renderer) {
  SDL_Color swap = to_sdl_color(color);
  SDL_Rect rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.w = w;
  rectangle.h = h;
  swap_color(renderer, &swap);
  SDL_RenderFillRect(renderer, &rectangle);
  swap_color(renderer, &swap);
}

/**
 * Draws a shaded absolute rectangle based on the provided coordinates.
 */
static void draw_shaded_absolute_rectangle(const int x, const int y,
                                           const int w, const int h,
                                           Color color, Renderer *renderer) {

  SDL_Color swap = to_sdl_color(color);
  SDL_Rect rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.w = w;
  rectangle.h = h;
  swap_color(renderer, &swap);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_RenderFillRect(renderer, &rectangle);
  swap_color(renderer, &swap);
}

static void draw_absolute_tile_rectangle(int x, int y, Color color,
                                         Renderer *renderer) {
  const int w = get_tile_width();
  const int h = get_tile_height();
  x += get_border_width();
  y += get_bar_height() + get_border_height();
  draw_absolute_rectangle(x, y, w, h, color, renderer);
}

static void draw_shaded_absolute_tile_rectangle(int x, int y, Color color,
                                                Renderer *renderer) {
  const int w = get_tile_width();
  const int h = get_tile_height();
  x += get_border_width();
  y += get_bar_height() + get_border_height();
  draw_shaded_absolute_rectangle(x, y, w, h, color, renderer);
}

static void write_top_bar_strings(char *strings[], Renderer *renderer) {
  const ColorPair color_pair = COLOR_PAIR_TOP_BAR;
  const int string_count = TOP_BAR_STRING_COUNT;
  const int y = (get_bar_height() - get_font_height()) / 2;
  int h = get_bar_height();
  int w = get_window_width();
  draw_absolute_rectangle(0, 0, w, h, color_pair.background, renderer);
  print_centered_horizontally(y, string_count, strings, color_pair, renderer);
}

/**
 * Draws the top status bar on the screen for a given Player.
 */
static void draw_top_bar(const Player *const player, Renderer *renderer) {
  char lives_buffer[MAXIMUM_STRING_SIZE];
  char score_buffer[MAXIMUM_STRING_SIZE];
  char *strings[TOP_BAR_STRING_COUNT];
  char *perk_name = "No Power";
  if (player->perk != PERK_NONE) {
    perk_name = get_perk_name(player->perk);
  }
  sprintf(lives_buffer, "Lives: %d", player->lives);
  sprintf(score_buffer, "Score: %ld", player->score);
  strings[0] = GAME_NAME;
  strings[1] = perk_name;
  strings[2] = lives_buffer;
  strings[3] = score_buffer;
  write_top_bar_strings(strings, renderer);
}

static void write_bottom_bar_string(const char *string, Renderer *renderer) {
  /* Use half a character for horizontal padding. */
  const int x = get_font_width() / 2;
  const int bar_start = get_window_height() - get_bar_height();
  const int padding = (get_bar_height() - get_font_height()) / 2;
  const int y = bar_start + padding;
  print_absolute(x, y, string, COLOR_PAIR_BOTTOM_BAR, renderer);
}

/*
 * Draws the bottom status bar on the screen for a given Player.
 */
static void draw_bottom_bar(const char *message, Renderer *renderer) {
  const Color color = COLOR_PAIR_BOTTOM_BAR.background;
  const int y = get_window_height() - get_bar_height();
  const int w = get_window_width();
  const int h = get_bar_height();
  draw_absolute_rectangle(0, y, w, h, color, renderer);
  write_bottom_bar_string(message, renderer);
}

/**
 * Draws the borders of the screen.
 */
static void draw_borders(Renderer *renderer) { render_borders(renderer); }

static void draw_platforms(const Platform *platforms,
                           const size_t platform_count, const BoundingBox *box,
                           Renderer *renderer) {
  const Color color = COLOR_PAIR_PLATFORM.foreground;
  const int x_padding = get_border_width();
  const int y_padding = get_bar_height() + get_border_height();
  Platform p;
  int x;
  int y;
  int w;
  int h;
  size_t i;
  for (i = 0; i < platform_count; i++) {
    p = platforms[i];
    x = x_padding + max_int(box->min_x, p.x);
    y = y_padding + p.y;
    w = min_int(box->max_x, p.x + p.w - 1) - (x - x_padding) + 1;
    h = p.h;
    draw_absolute_rectangle(x, y, w, h, color, renderer);
  }
}

static int has_active_perk(const Game *const game) {
  return game->perk != PERK_NONE;
}

static void draw_resized_perk(int x, int y, int w, int h, double f,
                              Renderer *renderer) {
  /* The scaled values. */
  const int s_w = (int)(f * w);
  const int s_h = (int)(f * h);
  /* The foreground variables. */
  const Color f_color = COLOR_PAIR_PERK.background;
  int f_x;
  int f_y;
  int f_w = s_w;
  int f_h = s_h;
  /* The background variables. */
  const Color b_color = mix_colors(COLOR_PAIR_DEFAULT.background, f_color);
  int b_x;
  int b_y;
  int b_w = s_w;
  int b_h = s_h;
  /* If the width or height is reduced by 2, reduce back and front. */
  /* Otherwise, reduce inner on each side and interpolate back. */
  if ((w - s_w) % 2 == 1) {
    b_w = s_w + 1;
    f_w = s_w - 1;
  }
  if ((h - s_h) % 2 == 1) {
    b_h = s_h + 1;
    f_h = s_h - 1;
  }
  f_x = x + (w - f_w) / 2;
  f_y = y + (h - f_h) / 2;
  b_x = x + (w - b_w) / 2;
  b_y = y + (h - b_h) / 2;
  draw_absolute_rectangle(b_x, b_y, b_w, b_h, b_color, renderer);
  draw_absolute_rectangle(f_x, f_y, f_w, f_h, f_color, renderer);
}

static void draw_active_perk(const Game *const game, Renderer *renderer) {
  const int interval = PERK_FADING_INTERVAL;
  const int x_padding = get_border_width();
  const int y_padding = get_bar_height() + get_border_height();
  const int remaining = (int)(game->perk_end_frame - game->played_frames);
  const double fraction = min_int(interval, remaining) / (double)interval;
  const int x = x_padding + game->perk_x;
  const int y = y_padding + game->perk_y;
  draw_resized_perk(x, y, game->tile_w, game->tile_h, fraction, renderer);
}

static void draw_perk(const Game *const game, Renderer *renderer) {
  if (has_active_perk(game)) {
    draw_active_perk(game, renderer);
  }
}

Code draw_player(const Player *const player, Renderer *renderer) {
  int x = player->x;
  int y = player->y;
  size_t i;
  const size_t head = player->graphics->trail_head;
  const size_t size = player->graphics->trail_size;
  const size_t capacity = player->graphics->trail_capacity;
  Color color = COLOR_PAIR_PLAYER.foreground;
  draw_absolute_tile_rectangle(x, y, color, renderer);
  for (i = 0; i < size; i++) {
    x = player->graphics->trail[(head + i) % capacity].x;
    y = player->graphics->trail[(head + i) % capacity].y;
    color.a = (unsigned char)((i + 1) * (255.0 / (capacity + 1)));
    draw_shaded_absolute_tile_rectangle(x, y, color, renderer);
  }
  return CODE_OK;
}

/**
 * Draws a full game to the screen.
 *
 * Returns a Milliseconds approximation of the time this function took.
 */
Milliseconds draw_game(const Game *const game, Renderer *renderer) {
  Milliseconds draw_game_start = get_milliseconds();

  profiler_begin("draw_game:clear");
  clear(renderer);
  profiler_end("draw_game:clear");

  profiler_begin("draw_game:draw_top_bar");
  draw_top_bar(game->player, renderer);
  profiler_end("draw_game:draw_top_bar");

  profiler_begin("draw_game:draw_bottom_bar");
  draw_bottom_bar(game->message, renderer);
  profiler_end("draw_game:draw_bottom_bar");

  profiler_begin("draw_game:draw_borders");
  draw_borders(renderer);
  profiler_end("draw_game:draw_borders");

  profiler_begin("draw_game:draw_platforms");
  draw_platforms(game->platforms, game->platform_count, game->box, renderer);
  profiler_end("draw_game:draw_platforms");

  profiler_begin("draw_game:draw_perk");
  draw_perk(game, renderer);
  profiler_end("draw_game:draw_perk");

  profiler_begin("draw_game:draw_player");
  draw_player(game->player, renderer);
  profiler_end("draw_game:draw_player");

  profiler_begin("draw_game:present");
  present(renderer);
  profiler_end("draw_game:present");

  update_profiler("draw_game", get_milliseconds() - draw_game_start);
  return get_milliseconds() - draw_game_start;
}

/**
 * Converts a Record to a human-readable string.
 */
static void record_to_string(const Record *const record, char *dest,
                             const int width) {
  const char format[] = "%s%*.*s%d";
  const char *name = record->name;
  const int score = record->score;
  char pad_string[MAXIMUM_STRING_SIZE];
  int pad_length;
  memset(pad_string, '.', MAXIMUM_STRING_SIZE - 1);
  pad_string[MAXIMUM_STRING_SIZE - 1] = '\0';
  pad_length = width - strlen(name) - count_digits(score);
  sprintf(dest, format, name, pad_length, pad_length, pad_string, score);
}

void print_records(const size_t count, const Record *records,
                   Renderer *renderer) {
  const ColorPair pair = COLOR_PAIR_DEFAULT;
  const int x_padding = 2 * get_padding() * get_font_width();
  const int y_padding = 2 * get_padding() * get_font_height();
  const int available_window_height = get_window_height() - y_padding;
  const int text_lines_limit = available_window_height / get_font_height();
  const int text_width_in_pixels = get_window_width() - x_padding;
  const size_t string_width = text_width_in_pixels / get_font_width();
  const size_t printed = min_int(count, text_lines_limit);
  char **strings = NULL;
  size_t i;
  strings = resize_memory(strings, sizeof(char *) * printed);
  for (i = 0; i < printed; i++) {
    strings[i] = NULL;
    strings[i] = resize_memory(strings[i], string_width);
    record_to_string(records + i, strings[i], string_width - 1);
  }
  clear(renderer);
  print_centered_vertically(printed, strings, pair, renderer);
  present(renderer);
  for (i = 0; i < printed; i++) {
    resize_memory(strings[i], 0);
  }
  resize_memory(strings, 0);
}

/**
 * Asserts whether or not a character is a valid input character.
 *
 * For simplicity, the user should only be able to enter letters and numbers.
 */
static int is_valid_input_character(char c) { return isalnum(c); }

/**
 * Prints a string starting from (x, y) but limits to its first limit
 * characters.
 */
static void print_limited(const int x, const int y, const char *string,
                          const size_t limit, Renderer *renderer) {
  const char ellipsis[] = "...";
  const size_t string_length = strlen(string);
  const size_t ellipsis_length = strlen(ellipsis);
  const size_t minimum_string_size_for_ellipsis = 2 * ellipsis_length;
  /* No-op. */
  if (limit < 1) {
    return;
  }
  /*
   * As only two calls to print suffice to solve this problem for any possible
   * input size, we avoid using a dynamically allocated buffer to prepare the
   * output.
   */
  /* String length is less than the limit. */
  if (string_length < limit) {
    print_absolute(x, y, string, COLOR_PAIR_DEFAULT, renderer);
    return;
  }
  /* String is longer than the limit. */
  /* Write the ellipsis if we need to. */
  if (limit >= minimum_string_size_for_ellipsis) {
    print_absolute(x, y, ellipsis, COLOR_PAIR_DEFAULT, renderer);
  }
  /* Write the tail of the input string. */
  string += string_length - limit + ellipsis_length;
  print_absolute(x + ellipsis_length, y, string, COLOR_PAIR_DEFAULT, renderer);
}

/**
 * Reads a string from the user of up to size characters (including NUL).
 */
Code read_string(const int x, const int y, const char *prompt,
                 char *destination, const size_t size, Renderer *renderer) {
  const int buffer_x = x + (strlen(prompt) + 1) * get_font_width();
  const int padding_size = get_padding() * get_font_width();
  const int buffer_view_size = get_window_width() - buffer_x - padding_size;
  const int buffer_view_limit = buffer_view_size / get_font_width();
  int is_done = 0;
  int should_rerender = 1;
  /* The x coordinate of the user input buffer. */
  size_t written = strlen(destination);
  char character = '\0';
  char *write = destination + written;
  SDL_Event event;
  /* Start listening for text input. */
  SDL_StartTextInput();
  while (!is_done) {
    if (should_rerender) {
      clear(renderer);
      print_absolute(x, y, prompt, COLOR_PAIR_DEFAULT, renderer);
      if (written == 0) {
        /* We must write a single space, or SDL will not render anything. */
        print_absolute(buffer_x, y, " ", COLOR_PAIR_DEFAULT, renderer);
      } else {
        /*
         * Must care about how much we write, padding should be respected.
         *
         * Simply enough, we print the tail of the string and omit the
         * beginning with an ellipsis.
         *
         */
        print_limited(buffer_x, y, destination, buffer_view_limit, renderer);
      }
      present(renderer);
      should_rerender = 0;
    }
    /* Throughout the loop, the write pointer always points to a '\0'. */
    if (SDL_WaitEvent(&event)) {
      /* Check for user quit and return 1. */
      /* This is OK because the destination string is always a valid C string.
       */
      if (event.type == SDL_QUIT) {
        return CODE_QUIT;
      } else if (event.type == SDL_KEYDOWN) {
        /* Handle backspace. */
        if (event.key.keysym.sym == SDLK_BACKSPACE && written > 0) {
          write--;
          *write = '\0';
          written--;
          should_rerender = 1;
        } else if (event.key.keysym.sym == SDLK_RETURN) {
          is_done = 1;
        }
      } else if (event.type == SDL_TEXTINPUT) {
        character = event.text.text[0];
        if (is_valid_input_character(character) && written + 1 < size) {
          *write = character;
          write++;
          written++;
          /* Terminate the string with NIL to ensure it is valid. */
          *write = '\0';
          should_rerender = 1;
        }
        /* We could handle copying and pasting by checking for Control-C. */
        /* SDL_*ClipboardText() allows us to access the clipboard text. */
      }
    }
  }
  /* Stop listening for text input. */
  SDL_StopTextInput();
  return CODE_OK;
}
