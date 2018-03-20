#include "base-io.hpp"
#include "clock.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "joystick.hpp"
#include "logger.hpp"
#include "memory.hpp"
#include "numeric.hpp"
#include "physics.hpp"
#include "player.hpp"
#include "profiler.hpp"
#include "random.hpp"
#include "record.hpp"
#include "settings.hpp"
#include "text.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cctype>
#include <cstdio>
#include <cstring>

#define CREATE_SURFACE_FAIL "Failed to create surface in %s!"
#define CREATE_TEXTURE_FAIL "Failed to create texture in %s!"

#define PERK_FADING_INTERVAL UPS

#define IMG_FLAGS IMG_INIT_PNG

#define SDL_INIT_FLAGS (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)

const char *game_name = "Walls of Doom";

static Font *global_monospaced_font = nullptr;

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

Font *get_font() { return global_monospaced_font; }

int get_font_width() { return global_monospaced_font_width; }

int get_font_height() { return global_monospaced_font_height; }

/**
 * Swap the renderer color by the provided color.
 *
 * The color previously in the renderer will be copied to the pointer.
 */
void swap_color(Renderer *renderer, SDL_Color *color) {
  SDL_Color swap{};
  SDL_GetRenderDrawColor(renderer, &swap.r, &swap.g, &swap.b, &swap.a);
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
  *color = swap;
}

/**
 * Initializes the global fonts.
 */
static Code initialize_fonts() {
  char log_buffer[MAXIMUM_STRING_SIZE];
  Font *font = nullptr;
  if (global_monospaced_font != nullptr) {
    return CODE_OK;
  }
  /* We try to open the font if we need to initialize. */
  font = TTF_OpenFont(MONOSPACED_FONT_PATH, get_font_size());
  /* If it failed, we log an error. */
  if (font == nullptr) {
    sprintf(log_buffer, "TTF font opening error: %s.", SDL_GetError());
    log_message(log_buffer);
    return CODE_ERROR;
  }
  global_monospaced_font = font;

  return CODE_OK;
}

/**
 * Initializes the required font metrics.
 */
static Code initialize_font_metrics() {
  int width;
  int height;
  Font *font = global_monospaced_font;
  if (TTF_GlyphMetrics(font, 'A', nullptr, nullptr, nullptr, nullptr, &width) != 0) {
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
  const int x = SDL_WINDOWPOS_CENTERED;
  const int y = SDL_WINDOWPOS_CENTERED;
  const int w = get_window_width();
  const int h = get_window_height();
  const Uint32 flags = SDL_WINDOW_INPUT_FOCUS;
  Window *window;
  window = SDL_CreateWindow(game_name, x, y, w, h, flags);
  SDL_GetWindowSize(window, width, height);
  return window;
}

static Code set_window_title_and_icon(Window *window) {
  SDL_Surface *icon_surface = IMG_Load(ICON_PATH);
  SDL_SetWindowTitle(window, game_name);
  if (icon_surface == nullptr) {
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
  if (SDL_Init(SDL_INIT_FLAGS) != 0) {
    sprintf(log_buffer, "SDL initialization error: %s.", SDL_GetError());
    log_message(log_buffer);
    return CODE_ERROR;
  }
  SDL_ShowCursor(SDL_DISABLE);
  initialize_joystick();
  if (TTF_WasInit() == 0) {
    if (TTF_Init() != 0) {
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
  if (*window == nullptr) {
    sprintf(log_buffer, "SDL initialization error: %s.", SDL_GetError());
    log_message(log_buffer);
    return CODE_ERROR;
  }
  if (initialize_fonts() != 0u) {
    sprintf(log_buffer, "Failed to initialize fonts.");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  if (initialize_font_metrics() != 0u) {
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
static void finalize_fonts() {
  if (global_monospaced_font != nullptr) {
    TTF_CloseFont(global_monospaced_font);
    global_monospaced_font = nullptr;
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
  *window = nullptr;
  if (TTF_WasInit() != 0) {
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
  SDL_Rect position{};
  position.x = x;
  position.y = y;
  if (string == nullptr || string[0] == '\0') {
    return CODE_OK;
  }
  /* Validate that x and y are nonnegative. */
  if (x < 0 || y < 0) {
    return CODE_ERROR;
  }
  surface = TTF_RenderText_Shaded(font, string, foreground, background);
  if (surface == nullptr) {
    sprintf(log_buffer, CREATE_SURFACE_FAIL, "print_absolute()");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    sprintf(log_buffer, CREATE_TEXTURE_FAIL, "print_absolute()");
    log_message(log_buffer);
    return CODE_ERROR;
  }
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(texture, nullptr, nullptr, &position.w, &position.h);
  SDL_RenderCopy(renderer, texture, nullptr, &position);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  return CODE_OK;
}

static void remove_first_breaks(char *string) {
  char c;
  size_t i;
  int preserve = 0;
  if (string == nullptr) {
    return;
  }
  for (i = 0; string[i] != '\0'; i++) {
    c = string[i];
    if (c == '\n') {
      if (preserve == 0) {
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
  SDL_Rect position{};
  position.x = get_padding() * font_width;
  position.y = get_padding() * font_width;
  clear(renderer);
  /* Validate that the string is not empty and that x and y are nonnegative. */
  if (string == nullptr || string[0] == '\0') {
    return;
  }
  remove_first_breaks(string);
  surface = TTF_RenderText_Blended_Wrapped(font, string, color, width);
  if (surface == nullptr) {
    sprintf(log_buffer, CREATE_SURFACE_FAIL, "print_long_text()");
    log_message(log_buffer);
    return;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == nullptr) {
    sprintf(log_buffer, CREATE_TEXTURE_FAIL, "print_long_text()");
    log_message(log_buffer);
    return;
  }
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(texture, nullptr, nullptr, &position.w, &position.h);
  SDL_RenderCopy(renderer, texture, nullptr, &position);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  present(renderer);
}

/**
 * Prints the provided strings centered at the specified absolute line.
 */
Code print_centered_horizontally(const int y, const std::vector<std::string> &strings, const ColorPair color_pair,
                                 Renderer *renderer) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  const SDL_Color foreground = to_sdl_color(color_pair.foreground);
  const SDL_Color background = to_sdl_color(color_pair.background);
  const int slice_size = get_window_width() / strings.size();
  Font *font = global_monospaced_font;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Rect position{};
  position.x = 0;
  position.y = y;
  /* Validate that x and y are nonnegative. */
  if (y < 0) {
    return CODE_ERROR;
  }
  for (int i = 0; i < static_cast<int>(strings.size()); i++) {
    surface = TTF_RenderText_Shaded(font, strings[i].c_str(), foreground, background);
    if (surface == nullptr) {
      sprintf(log_buffer, CREATE_SURFACE_FAIL, "print_centered_horizontally()");
      log_message(log_buffer);
      return CODE_ERROR;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
      sprintf(log_buffer, CREATE_TEXTURE_FAIL, "print_centered_horizontally()");
      log_message(log_buffer);
      return CODE_ERROR;
    }
    /* Copy destination width and height from the texture. */
    SDL_QueryTexture(texture, nullptr, nullptr, &position.w, &position.h);
    position.x = i * slice_size + (slice_size - position.w) / 2;
    SDL_RenderCopy(renderer, texture, nullptr, &position);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
  }
  return CODE_OK;
}

/**
 * Prints the provided strings centered in the middle of the screen.
 */
Code print_centered_vertically(const std::vector<std::string> &strings, const ColorPair color_pair,
                               Renderer *renderer) {
  const int text_line_height = global_monospaced_font_height;
  const int padding = 2 * get_padding() * global_monospaced_font_height;
  const int available_window_height = get_window_height() - padding;
  const int text_lines_limit = available_window_height / text_line_height;
  const auto printed_count = std::min(text_lines_limit, static_cast<int>(strings.size()));
  auto y = (get_window_height() - strings.size() * text_line_height) / 2;
  for (int i = 0; i < printed_count; i++) {
    const auto string = strings[i];
    auto *array = reinterpret_cast<char *>(malloc(string.size() + 1));
    copy_string(array, string.c_str(), string.size() + 1);
    print_centered_horizontally(y, {string}, color_pair, renderer);
    y += text_line_height;
    free(array);
  }
  return CODE_OK;
}
