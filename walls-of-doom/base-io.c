#include "base-io.h"
#include "clock.h"
#include "constants.h"
#include "game.h"
#include "joystick.h"
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

#define CREATE_SURFACE_FAIL "Failed to create surface in %s!"
#define CREATE_TEXTURE_FAIL "Failed to create texture in %s!"

#define PERK_FADING_INTERVAL FPS

#define IMG_FLAGS IMG_INIT_PNG

#define SDL_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)

static Font *global_monospaced_font = NULL;

/* Default integers to one to prevent divisions by zero. */
static int window_width = 1;
static int window_height = 1;
static int global_monospaced_font_width = 1;
static int global_monospaced_font_height = 1;

/**
 * Clears the screen.
 */
void clear(Renderer *renderer) { SDL_RenderClear(renderer); }

/**
 * Updates the screen with what has been rendered.
 */
void present(Renderer *renderer) { SDL_RenderPresent(renderer); }

Font *get_font(void) { return global_monospaced_font; }

int get_font_width(void) { return global_monospaced_font_width; }

int get_font_height(void) { return global_monospaced_font_height; }

/**
 * Swap the renderer color by the provided color.
 *
 * The color previously in the renderer will be copied to the pointer.
 */
void swap_color(Renderer *renderer, SDL_Color *color) {
  SDL_Color swap;
  SDL_GetRenderDrawColor(renderer, &swap.r, &swap.g, &swap.b, &swap.a);
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
  *color = swap;
}

/**
 * Initializes the global fonts.
 */
static Code initialize_fonts(void) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  Font *font = NULL;
  if (global_monospaced_font != NULL) {
    return CODE_OK;
  }
  /* We try to open the font if we need to initialize. */
  font = TTF_OpenFont(MONOSPACED_FONT_PATH, get_font_size());
  /* If it failed, we log an error. */
  if (font == NULL) {
    sprintf(log_buffer, "TTF font opening error: %s.", SDL_GetError());
    log_message(log_buffer);
    return CODE_ERROR;
  } else {
    global_monospaced_font = font;
  }
  return CODE_OK;
}

/**
 * Initializes the required font metrics.
 */
static Code initialize_font_metrics(void) {
  int width;
  int height;
  Font *font = global_monospaced_font;
  if (TTF_GlyphMetrics(font, 'A', NULL, NULL, NULL, NULL, &width)) {
    log_message("Could not assess the width of a font.");
    return CODE_ERROR;
  }
  height = TTF_FontHeight(font);
  global_monospaced_font_width = width;
  global_monospaced_font_height = height;
  return CODE_OK;
}

/**
 * Creates a new fullscreen window.
 *
 * Updates the pointers with the window width, window height, and bar height.
 */
static Window *create_window(int *width, int *height) {
  const char *title = GAME_NAME;
  const int x = SDL_WINDOWPOS_CENTERED;
  const int y = SDL_WINDOWPOS_CENTERED;
  const int w = get_window_width();
  const int h = get_window_height();
  const Uint32 flags = SDL_WINDOW_INPUT_FOCUS;
  Window *window;
  window = SDL_CreateWindow(title, x, y, w, h, flags);
  SDL_GetWindowSize(window, width, height);
  return window;
}

static Code set_window_title_and_icon(Window *window) {
  SDL_Surface *icon_surface = IMG_Load(ICON_PATH);
  SDL_SetWindowTitle(window, GAME_NAME);
  if (icon_surface == NULL) {
    log_message("Failed to load the icon.");
    return CODE_ERROR;
  }
  SDL_SetWindowIcon(window, icon_surface);
  SDL_FreeSurface(icon_surface);
  return CODE_OK;
}

static void set_color(Renderer *renderer, Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 */
Code initialize(Window **window, Renderer **renderer) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  Uint32 renderer_flags = 0;
  initialize_logger();
  initialize_profiler();
  initialize_settings();
  if (SDL_Init(SDL_INIT_FLAGS)) {
    sprintf(log_buffer, "SDL initialization error: %s.", SDL_GetError());
    log_message(log_buffer);
    return CODE_ERROR;
  }
  SDL_ShowCursor(SDL_DISABLE);
  initialize_joystick();
  if (!TTF_WasInit()) {
    if (TTF_Init()) {
      sprintf(log_buffer, "TTF initialization error: %s.", SDL_GetError());
      log_message(log_buffer);
      return CODE_ERROR;
    }
  }
  if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS) {
    sprintf(log_buffer, "Failed to initialize required image support.");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  /**
   * The number of columns and the number of lines are fixed. However, the
   * number of pixels we need for the screen is not. We find this number by
   * experimenting before creating the window.
   */
  /* Log the size of the window we are going to create. */
  *window = create_window(&window_width, &window_height);
  sprintf(log_buffer, "Created a %dx%d window.", window_width, window_height);
  log_message(log_buffer);
  if (*window == NULL) {
    sprintf(log_buffer, "SDL initialization error: %s.", SDL_GetError());
    log_message(log_buffer);
    return CODE_ERROR;
  }
  if (initialize_fonts()) {
    sprintf(log_buffer, "Failed to initialize fonts.");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  if (initialize_font_metrics()) {
    sprintf(log_buffer, "Failed to initialize font metrics.");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  /* Must disable text input to prevent a name capture bug. */
  SDL_StopTextInput();
  set_window_title_and_icon(*window);
  if (get_renderer_type() == RENDERER_HARDWARE) {
    renderer_flags = SDL_RENDERER_ACCELERATED;
  } else {
    renderer_flags = SDL_RENDERER_SOFTWARE;
  }
  *renderer = SDL_CreateRenderer(*window, -1, renderer_flags);
  set_color(*renderer, COLOR_DEFAULT_BACKGROUND);
  clear(*renderer);
  return CODE_OK;
}

/**
 * Finalizes the global fonts.
 */
static void finalize_fonts(void) {
  if (global_monospaced_font != NULL) {
    TTF_CloseFont(global_monospaced_font);
    global_monospaced_font = NULL;
  }
}

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exiting.
 */
Code finalize(Window **window, Renderer **renderer) {
  finalize_fonts();
  finalize_joystick();
  SDL_DestroyRenderer(*renderer);
  SDL_DestroyWindow(*window);
  *window = NULL;
  if (TTF_WasInit()) {
    TTF_Quit();
  }
  /* This could be called earlier, but we only do it here to organize things. */
  IMG_Quit();
  SDL_Quit();
  finalize_profiler();
  finalize_logger();
  return CODE_OK;
}

Code print_absolute(const int x, const int y, const char *string, const ColorPair color_pair, Renderer *renderer) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  const SDL_Color foreground = to_sdl_color(color_pair.foreground);
  const SDL_Color background = to_sdl_color(color_pair.background);
  Font *font = global_monospaced_font;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Rect position;
  position.x = x;
  position.y = y;
  if (string == NULL || string[0] == '\0') {
    return CODE_OK;
  }
  /* Validate that x and y are nonnegative. */
  if (x < 0 || y < 0) {
    return CODE_ERROR;
  }
  surface = TTF_RenderText_Shaded(font, string, foreground, background);
  if (surface == NULL) {
    sprintf(log_buffer, CREATE_SURFACE_FAIL, "print_absolute()");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    sprintf(log_buffer, CREATE_TEXTURE_FAIL, "print_absolute()");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
  SDL_RenderCopy(renderer, texture, NULL, &position);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  return CODE_OK;
}

static void remove_first_breaks(char *string) {
  char c;
  size_t i;
  int preserve = 0;
  if (string == NULL) {
    return;
  }
  for (i = 0; string[i] != '\0'; i++) {
    c = string[i];
    if (c == '\n') {
      if (!preserve) {
        string[i] = ' ';
        /* Stop erasing newlines. */
        preserve = 1;
      }
    } else {
      /* Not a newline, we can erase again. */
      preserve = 0;
    }
  }
}

/**
 * Prints the provided string after formatting it to increase readability.
 */
void print_long_text(char *string, Renderer *renderer) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  const int font_width = get_font_width();
  const int width = get_window_width() - 2 * get_padding() * font_width;
  TTF_Font *font = get_font();
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Color color = to_sdl_color(COLOR_DEFAULT_FOREGROUND);
  SDL_Rect position;
  position.x = get_padding() * font_width;
  position.y = get_padding() * font_width;
  clear(renderer);
  /* Validate that the string is not empty and that x and y are nonnegative. */
  if (string == NULL || string[0] == '\0') {
    return;
  }
  remove_first_breaks(string);
  surface = TTF_RenderText_Blended_Wrapped(font, string, color, width);
  if (surface == NULL) {
    sprintf(log_buffer, CREATE_SURFACE_FAIL, "print_long_text()");
    log_message(log_buffer);
    return;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    sprintf(log_buffer, CREATE_TEXTURE_FAIL, "print_long_text()");
    log_message(log_buffer);
    return;
  }
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
  SDL_RenderCopy(renderer, texture, NULL, &position);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  present(renderer);
}

/**
 * Prints the provided strings centered at the specified absolute line.
 */
Code print_centered_horizontally(const int y, const int string_count, char **strings, const ColorPair color_pair,
                                 Renderer *renderer) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  const SDL_Color foreground = to_sdl_color(color_pair.foreground);
  const SDL_Color background = to_sdl_color(color_pair.background);
  const int slice_size = get_window_width() / string_count;
  Font *font = global_monospaced_font;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Rect position;
  int i;
  position.x = 0;
  position.y = y;
  /* Validate that x and y are nonnegative. */
  if (y < 0) {
    return CODE_ERROR;
  }
  for (i = 0; i < string_count; i++) {
    if (*strings[i] == '\0') {
      continue;
    }
    surface = TTF_RenderText_Shaded(font, strings[i], foreground, background);
    if (surface == NULL) {
      sprintf(log_buffer, CREATE_SURFACE_FAIL, "print_centered_horizontally()");
      log_message(log_buffer);
      return CODE_ERROR;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
      sprintf(log_buffer, CREATE_TEXTURE_FAIL, "print_centered_horizontally()");
      log_message(log_buffer);
      return CODE_ERROR;
    }
    /* Copy destination width and height from the texture. */
    SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
    position.x = i * slice_size + (slice_size - position.w) / 2;
    SDL_RenderCopy(renderer, texture, NULL, &position);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
  }
  return CODE_OK;
}

/**
 * Prints the provided strings centered in the middle of the screen.
 */
Code print_centered_vertically(const int string_count, char **strings, const ColorPair color_pair, Renderer *renderer) {
  const int text_line_height = global_monospaced_font_height;
  const int padding = 2 * get_padding() * global_monospaced_font_height;
  const int available_window_height = get_window_height() - padding;
  const int text_lines_limit = available_window_height / text_line_height;
  int printed_count = string_count;
  int y;
  int i;
  if (string_count > text_lines_limit) {
    printed_count = text_lines_limit;
  }
  y = (get_window_height() - string_count * text_line_height) / 2;
  for (i = 0; i < printed_count; i++) {
    print_centered_horizontally(y, 1, strings + i, color_pair, renderer);
    y += text_line_height;
  }
  return CODE_OK;
}
