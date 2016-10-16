#include "io.h"

#include "clock.h"
#include "constants.h"
#include "game.h"
#include "graphics.h"
#include "logger.h"
#include "memory.h"
#include "numeric.h"
#include "physics.h"
#include "player.h"
#include "profiler.h"
#include "random.h"
#include "settings.h"
#include "text.h"

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define GAME_NAME "Walls of Doom"

#define ELLIPSIS_STRING "..."
#define ELLIPSIS_LENGTH (strlen(ELLIPSIS_STRING))
#define MINIMUM_STRING_SIZE_FOR_ELLIPSIS (2 * ELLIPSIS_LENGTH)

#define IMG_FLAGS IMG_INIT_PNG

static SDL_GLContext context;

static TTF_Font *global_monospaced_font = NULL;
static int global_monospaced_font_width = 0;
static int global_monospaced_font_height = 0;
static SDL_Texture *borders_texture = NULL;
static GLuint platform_vertex_buffer_object = 0;

/**
 * Clears the screen.
 */
void clear(SDL_Renderer *renderer) { SDL_RenderClear(renderer); }

/**
 * Updates the screen with what has been rendered.
 */
void present(SDL_Renderer *renderer) { SDL_RenderPresent(renderer); }

/**
 * Initializes the global fonts.
 *
 * Returns 0 in case of success.
 */
static int initialize_fonts(void) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  TTF_Font *font = NULL;
  if (global_monospaced_font != NULL) {
    return 0;
  }
  /* We try to open the font if we need to initialize. */
  font = TTF_OpenFont(MONOSPACED_FONT_PATH, get_font_size());
  /* If it failed, we log an error. */
  if (font == NULL) {
    sprintf(log_buffer, "TTF font opening error: %s", SDL_GetError());
    log_message(log_buffer);
    return 1;
  } else {
    global_monospaced_font = font;
  }
  return 0;
}

/**
 * Initializes the required font metrics.
 *
 * Returns 0 in case of success.
 */
static int initialize_font_metrics(void) {
  int width;
  int height;
  TTF_Font *font = global_monospaced_font;
  if (TTF_GlyphMetrics(font, 'A', NULL, NULL, NULL, NULL, &width)) {
    log_message("Could not assess the width of a font");
    return 1;
  }
  height = TTF_FontHeight(font);
  global_monospaced_font_width = width;
  global_monospaced_font_height = height;
  return 0;
}

static SDL_Window *create_window(int width, int height) {
  char *title = GAME_NAME;
  int x = SDL_WINDOWPOS_CENTERED;
  int y = SDL_WINDOWPOS_CENTERED;
  Uint32 flags = SDL_WINDOW_OPENGL;
  return SDL_CreateWindow(title, x, y, width, height, flags);
}

int set_window_title_and_icon(SDL_Window *window) {
  SDL_Surface *icon_surface = IMG_Load(ICON_PATH);
  SDL_SetWindowTitle(window, GAME_NAME);
  if (icon_surface == NULL) {
    log_message("Failed to load the window icon");
    return 1;
  }
  SDL_SetWindowIcon(window, icon_surface);
  SDL_FreeSurface(icon_surface);
  return 0;
}

static void set_render_color(SDL_Renderer *renderer, Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

static void initialize_graphics(void) {
  glewExperimental = GL_TRUE;
  glewInit();
  glGenBuffers(1, &platform_vertex_buffer_object);
  glBindBuffer(GL_ARRAY_BUFFER, platform_vertex_buffer_object);
  initialize_shaders();
}

/**
 * Initializes the required resources.
 *
 * Should only be called once, right after starting.
 *
 * Returns 0 in case of success.
 */
int initialize(SDL_Window **window, SDL_Renderer **renderer) {
  char log_buffer[MAXIMUM_STRING_SIZE];
  int width = 1;
  int height = 1;
  initialize_logger();
  initialize_profiler();
  initialize_settings();
  /* Initialize SDL. */
  if (SDL_Init(SDL_INIT_VIDEO)) {
    sprintf(log_buffer, "SDL initialization error: %s", SDL_GetError());
    log_message(log_buffer);
    return 1;
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  /* Initialize TTF. */
  if (!TTF_WasInit()) {
    if (TTF_Init()) {
      sprintf(log_buffer, "TTF initialization error: %s", SDL_GetError());
      log_message(log_buffer);
      return 1;
    }
  }
  if (initialize_fonts()) {
    sprintf(log_buffer, "Failed to initialize fonts");
    log_message(log_buffer);
    return 1;
  }
  if (initialize_font_metrics()) {
    sprintf(log_buffer, "Failed to initialize font metrics");
    log_message(log_buffer);
    return 1;
  }
  if ((IMG_Init(IMG_FLAGS) & IMG_FLAGS) != IMG_FLAGS) {
    sprintf(log_buffer, "Failed to initialize required image support");
    log_message(log_buffer);
    return 1;
  }
  /**
   * The number of columns and the number of lines are fixed. However, the
   * number of pixels we need for the screen is not. We find this number by
   * experimenting before creating the window.
   */
  width = global_monospaced_font_width * get_columns();
  height = global_monospaced_font_height * get_lines();
  /* Log the size of the window we are going to create. */
  sprintf(log_buffer, "Creating a %dx%d window", width, height);
  log_message(log_buffer);
  *window = create_window(width, height);
  if (*window == NULL) {
    sprintf(log_buffer, "SDL initialization error: %s", SDL_GetError());
    log_message(log_buffer);
    return 1;
  }
  set_window_title_and_icon(*window);
  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
  set_render_color(*renderer, COLOR_DEFAULT_BACKGROUND);
  clear(*renderer);
  context = SDL_GL_CreateContext(*window);
  initialize_graphics();
  return 0;
}

static void finalize_cached_textures(void) {
  SDL_DestroyTexture(borders_texture);
  borders_texture = NULL;
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
 *
 * Returns 0 in case of success.
 */
int finalize(SDL_Window **window, SDL_Renderer **renderer) {
  finalize_cached_textures();
  finalize_fonts();
  SDL_DestroyRenderer(*renderer);
  SDL_DestroyWindow(*window);
  *window = NULL;
  if (TTF_WasInit()) {
    TTF_Quit();
  }
  /* This could be called earlier, but we only do it here to organize things. */
  IMG_Quit();
  SDL_GL_DeleteContext(context);
  SDL_Quit();
  finalize_profiler();
  finalize_logger();
  return 0;
}

/**
 * Evaluates whether or not a Player name is a valid name.
 *
 * A name is considered to be valid if it has at least two characters after
 * being trimmed.
 */
int is_valid_player_name(const char *player_name) {
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
                      SDL_Renderer *renderer) {
  int x;
  int y;
  Code code = CODE_ERROR;
  int valid_name = 0;
  const char message[] = "Name your character: ";
  char log_buffer[MAXIMUM_STRING_SIZE];
  random_name(destination);
  /* While there is not a read error or a valid name. */
  while (code != CODE_OK || !valid_name) {
    x = PADDING;
    y = get_lines() / 2;
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

/**
 * Prints the provided string on the screen starting at (x, y).
 *
 * Returns 0 in case of success.
 */
int print(const int x, const int y, const char *string,
          const ColorPair color_pair, SDL_Renderer *renderer) {
  const SDL_Color foreground = to_sdl_color(color_pair.foreground);
  const SDL_Color background = to_sdl_color(color_pair.background);
  TTF_Font *font = global_monospaced_font;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Rect position;
  position.x = global_monospaced_font_width * x;
  position.y = global_monospaced_font_height * y;
  if (string == NULL || string[0] == '\0') {
    return 0;
  }
  /* Validate that x and y are nonnegative. */
  if (x < 0 || y < 0) {
    return 1;
  }
  surface = TTF_RenderText_Shaded(font, string, foreground, background);
  if (surface == NULL) {
    log_message("Failed to allocate text surface in print()");
    return 1;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    log_message("Failed to create texture from surface in print()");
    return 1;
  }
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
  SDL_RenderCopy(renderer, texture, NULL, &position);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  return 0;
}

static SDL_Texture *renderable_texture(int w, int h, SDL_Renderer *renderer) {
  const int access = SDL_TEXTUREACCESS_TARGET;
  return SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, access, w, h);
}

static Code cache_borders_texture(BoundingBox borders, SDL_Renderer *renderer) {
  const SDL_Color foreground = to_sdl_color(COLOR_PAIR_DEFAULT.foreground);
  const SDL_Color background = to_sdl_color(COLOR_PAIR_DEFAULT.background);
  const int x_step = global_monospaced_font_width;
  const int y_step = global_monospaced_font_height;
  const int min_x = borders.min_x;
  const int max_x = borders.max_x;
  const int min_y = borders.min_y;
  const int max_y = borders.max_y;
  const int full_width = (max_x - min_x + 1) * x_step;
  const int full_height = (max_y - min_y + 1) * y_step;
  SDL_Surface *surface;
  SDL_Texture *glyph_texture;
  SDL_Texture *full_texture;
  SDL_Rect position;
  TTF_Font *font = global_monospaced_font;
  int x;
  int y;
  if (borders_texture != NULL) {
    return CODE_ERROR;
  }
  if (min_x < 0 || min_y < 0 || min_x > max_x || min_y > max_y) {
    log_message("Got invalid border limits");
    return CODE_ERROR;
  }
  /* All preconditions are valid. */
  /* Now we create a texture for a single glyph. */
  surface = TTF_RenderGlyph_Shaded(font, '+', foreground, background);
  glyph_texture = SDL_CreateTextureFromSurface(renderer, surface);
  /* Free surface now because we may return before the end. */
  SDL_FreeSurface(surface);
  /* Create the target texture. */
  full_texture = renderable_texture(full_width, full_height, renderer);
  if (!full_texture) {
    log_message("Failed to create cached borders texture");
    return CODE_ERROR;
  }
  /* Change the renderer target to the texture which will be cached. */
  SDL_SetRenderTarget(renderer, full_texture);
  SDL_RenderClear(renderer);
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(glyph_texture, NULL, NULL, &position.w, &position.h);
  /*
   * This is the optimized step.
   *
   * We have a texture of a single symbol, so we just copy it to all the places
   * where the symbol should appear.
   */
  /* Write the top and bottom borders. */
  for (x = 0; x < full_width; x += x_step) {
    position.x = x;
    position.y = 0;
    SDL_RenderCopy(renderer, glyph_texture, NULL, &position);
    position.y = full_height - y_step;
    SDL_RenderCopy(renderer, glyph_texture, NULL, &position);
  }
  /* Write the left and right borders. */
  for (y = 0; y < full_height; y += y_step) {
    position.y = y;
    position.x = 0;
    SDL_RenderCopy(renderer, glyph_texture, NULL, &position);
    position.x = full_width - x_step;
    SDL_RenderCopy(renderer, glyph_texture, NULL, &position);
  }
  /* Change the renderer target back to the window. */
  SDL_DestroyTexture(glyph_texture);
  SDL_SetRenderTarget(renderer, NULL);
  /* Note that the texture is not destroyed here (obviosly). */
  borders_texture = full_texture;
  return CODE_OK;
}

static Code render_borders(BoundingBox borders, SDL_Renderer *renderer) {
  const int x_step = global_monospaced_font_width;
  const int y_step = global_monospaced_font_height;
  SDL_Rect pos;
  pos.x = borders.min_x * x_step;
  pos.y = borders.min_y * y_step;
  if (borders_texture == NULL) {
    cache_borders_texture(borders, renderer);
    if (borders_texture == NULL) {
      /* Failed to cache the texture. */
      return CODE_ERROR;
    }
  }
  SDL_QueryTexture(borders_texture, NULL, NULL, &pos.w, &pos.h);
  SDL_RenderCopy(renderer, borders_texture, NULL, &pos);
  return CODE_OK;
}

/**
 * Prints the provided string centered on the screen at the provided line.
 */
void print_centered(const int y, const char *string, const ColorPair color_pair,
                    SDL_Renderer *renderer) {
  const int x = (get_columns() - strlen(string)) / 2;
  print(x, y, string, color_pair, renderer);
}

/**
 * Replaces the first line break of any sequence of line breaks by a space.
 */
static void remove_first_breaks(char *string) {
  char c;
  size_t i;
  int preserve = 0;
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

int count_lines(char *const buffer) {
  size_t counter = 0;
  size_t i = 0;
  while (buffer[i] != '\0') {
    if (buffer[i] == '\n') {
      counter++;
    }
    i++;
  }
  return counter;
}

char *copy_first_line(char *source, char *destination) {
  while (*source != '\0' && *source != '\n') {
    *destination++ = *source++;
  }
  *destination = '\0';
  if (*source == '\0') {
    return source;
  } else {
    return source + 1;
  }
}

/**
 * Prints the provided string after formatting it to increase readability.
 */
void print_long_text(char *string, SDL_Renderer *renderer) {
  const int font_width = global_monospaced_font_width;
  const int width = (get_columns() - 2 * PADDING) * font_width;
  TTF_Font *font = global_monospaced_font;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Color color = to_sdl_color(COLOR_DEFAULT_FOREGROUND);
  SDL_Rect position;
  position.x = PADDING * font_width;
  position.y = PADDING * font_width;
  remove_first_breaks(string);
  clear(renderer);
  /* Validate that the string is not empty and that x and y are nonnegative. */
  if (string == NULL || string[0] == '\0') {
    return;
  }
  surface = TTF_RenderText_Blended_Wrapped(font, string, color, width);
  if (surface == NULL) {
    log_message("Failed to allocate text surface in print()");
    return;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    log_message("Failed to create texture from surface in print()");
    return;
  }
  /* Copy destination width and height from the texture. */
  SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
  SDL_RenderCopy(renderer, texture, NULL, &position);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
  present(renderer);
}

void write_top_bar_strings(char *strings[], SDL_Renderer *renderer) {
  int begin_x;
  int after_x;
  int begin_text_x;
  int after_text_x;

  int string_length;

  int x;
  int i;

  const int columns_per_string = get_columns() / TOP_BAR_STRING_COUNT;

  char buffer[MAXIMUM_COLUMNS + 1];
  memset(buffer, ' ', get_columns());
  buffer[get_columns()] = '\0';

  for (i = 0; i < TOP_BAR_STRING_COUNT; i++) {
    begin_x = i * columns_per_string;
    after_x = (i + 1) * columns_per_string;
    if (i + 1 == TOP_BAR_STRING_COUNT) {
      /*
       * If this is the last string on the top, make sure that we will
       * write enough colored spaces. Integer division truncates, which
       * can make 4 * (COLUMNS / 4) != COLUMNS. Therefore, some spaces may be
       * left uncolored at the end if we do not ensure that all columns
       * are painted.
       */
      after_x = get_columns();
    }
    string_length = strlen(strings[i]);
    if (string_length < columns_per_string) {
      /*
       * Write the string because it fits.
       */
      begin_text_x = begin_x + (columns_per_string - string_length) / 2;
      after_text_x = begin_text_x + string_length;
      for (x = begin_x; x < begin_text_x; x++) {
        buffer[x] = ' ';
      }
      copy_string(buffer + x, strings[i], get_columns() + 1 - x);
      for (x = after_text_x; x < after_x; x++) {
        buffer[x] = ' ';
      }
    } else {
      /*
       * String does not fit, do not write it.
       */
      for (x = begin_x; x < after_x; x++) {
        buffer[x] = ' ';
      }
    }
  }
  print(0, 0, buffer, COLOR_PAIR_TOP_BAR, renderer);
}

/**
 * Draws the top status bar on the screen for a given Player.
 *
 * Returns 0 if successful.
 */
int draw_top_bar(const Player *const player, SDL_Renderer *renderer) {
  char power_buffer[MAXIMUM_STRING_SIZE];
  char lives_buffer[MAXIMUM_STRING_SIZE];
  char score_buffer[MAXIMUM_STRING_SIZE];

  char *strings[TOP_BAR_STRING_COUNT];

  if (player->perk != PERK_NONE) {
    sprintf(power_buffer, "%s", get_perk_name(player->perk));
  } else {
    sprintf(power_buffer, "No Power");
  }

  sprintf(lives_buffer, "Lives: %d", player->lives);

  sprintf(score_buffer, "Score: %d", player->score);

  strings[0] = GAME_NAME;
  strings[1] = power_buffer;
  strings[2] = lives_buffer;
  strings[3] = score_buffer;

  write_top_bar_strings(strings, renderer);
  return 0;
}

/*
 * Draws the bottom status bar on the screen for a given Player.
 */
void draw_bottom_bar(const char *message, SDL_Renderer *renderer) {
  char buffer[MAXIMUM_COLUMNS + 1];
  memset(buffer, ' ', get_columns());
  buffer[get_columns()] = '\0';
  print(0, get_lines() - 1, buffer, COLOR_PAIR_BOTTOM_BAR, renderer);
  print(0, get_lines() - 1, message, COLOR_PAIR_BOTTOM_BAR, renderer);
}

/**
 * Draws the borders of the screen.
 */
void draw_borders(SDL_Renderer *renderer) {
  BoundingBox borders;
  borders.min_x = 0;
  borders.max_x = get_columns() - 1;
  borders.min_y = 1;
  borders.max_y = get_lines() - 2;
  render_borders(borders, renderer);
}

static void draw_rectangle(SDL_Rect rectangle, ColorPair color_pair,
                           SDL_Renderer *renderer) {
  const SDL_Color color = to_sdl_color(color_pair.foreground);
  SDL_Color helper;
  SDL_GetRenderDrawColor(renderer, &helper.r, &helper.g, &helper.b, &helper.a);
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rectangle);
  SDL_SetRenderDrawColor(renderer, helper.r, helper.g, helper.b, helper.a);
}

static int draw_platforms(const Platform *platforms) {
  const float l = get_lines();
  const float c = get_columns();
  /* Just draw the first platform. */
  Platform p = platforms[0];
  float vertices[8];
  vertices[0] = p.x / c;
  vertices[1] = p.y / l;

  vertices[2] = (p.x + p.width) / c;
  vertices[3] = p.y / l;

  vertices[4] = (p.x + p.width) / c;
  vertices[5] = (p.y + 1) / l;

  vertices[6] = p.x / c;
  vertices[7] = (p.y + 1) / l;

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  return 0;
}

int has_active_perk(const Game *const game) { return game->perk != PERK_NONE; }

static ColorPair get_perk_color() { return COLOR_PAIR_PERK; }

int draw_perk(const Game *const game, SDL_Renderer *renderer) {
  ColorPair perk_color;
  if (has_active_perk(game)) {
    perk_color = get_perk_color();
    print(game->perk_x, game->perk_y, get_perk_symbol(), perk_color, renderer);
  }
  return 0;
}

Code draw_player(const Player *const player, SDL_Renderer *renderer) {
  const int w = global_monospaced_font_width;
  const int h = global_monospaced_font_height;
  SDL_Rect rectangle;
  rectangle.x = player->x * w;
  rectangle.y = player->y * h;
  rectangle.h = h;
  rectangle.w = w;
  draw_rectangle(rectangle, COLOR_PAIR_PLAYER, renderer);
  return CODE_OK;
}

/**
 * Draws a full game to the screen.
 *
 * Returns a Milliseconds approximation of the time this function took.
 */
Milliseconds draw_game(const Game *const game, SDL_Renderer *renderer) {
  Milliseconds draw_game_start = get_milliseconds();
  Milliseconds start;

  start = get_milliseconds();
  clear(renderer);
  update_profiler("draw_game:clear", get_milliseconds() - start);

  start = get_milliseconds();
  draw_top_bar(game->player, renderer);
  update_profiler("draw_game:draw_top_bar", get_milliseconds() - start);

  start = get_milliseconds();
  draw_bottom_bar(game->message, renderer);
  update_profiler("draw_game:draw_bottom_bar", get_milliseconds() - start);

  start = get_milliseconds();
  draw_borders(renderer);
  update_profiler("draw_game:draw_borders", get_milliseconds() - start);

  start = get_milliseconds();
  draw_platforms(game->platforms);
  update_profiler("draw_game:draw_platforms", get_milliseconds() - start);

  start = get_milliseconds();
  draw_perk(game, renderer);
  update_profiler("draw_game:draw_perk", get_milliseconds() - start);

  start = get_milliseconds();
  draw_player(game->player, renderer);
  update_profiler("draw_game:draw_player", get_milliseconds() - start);

  start = get_milliseconds();
  present(renderer);
  update_profiler("draw_game:present", get_milliseconds() - start);

  update_profiler("draw_game", get_milliseconds() - draw_game_start);
  return get_milliseconds() - draw_game_start;
}

void print_game_result(const char *name, const unsigned int score,
                       const int position, SDL_Renderer *renderer) {
  const ColorPair color = COLOR_PAIR_DEFAULT;
  char first_line[MAXIMUM_STRING_SIZE];
  char second_line[MAXIMUM_STRING_SIZE];
  sprintf(first_line, "%s died after making %d points.", name, score);
  if (position > 0) {
    sprintf(second_line, "%s got to position %d!", name, position);
  } else {
    sprintf(second_line, "%s didn't make it to the top scores.", name);
  }
  clear(renderer);
  print_centered(get_lines() / 2 - 1, first_line, color, renderer);
  print_centered(get_lines() / 2 + 1, second_line, color, renderer);
  present(renderer);
}

/**
 * Returns a BoundingBox that represents the playable area after removing bars
 * and margins.
 */
BoundingBox bounding_box_from_screen(void) {
  BoundingBox box;
  box.min_x = 1;
  box.min_y = 2; /* Top bar. */
  box.max_x = get_columns() - 2;
  box.max_y = get_lines() - 3; /* Bottom bar. */
  return box;
}

/**
 * Returns the Command value corresponding to the provided input code.
 */
Command command_from_event(const SDL_Event event) {
  SDL_Keycode keycode;
  if (event.type == SDL_QUIT) {
    return COMMAND_CLOSE;
  }
  if (event.type == SDL_KEYDOWN) {
    keycode = event.key.keysym.sym;
    if (keycode == SDLK_KP_8 || keycode == SDLK_UP) {
      return COMMAND_UP;
    } else if (keycode == SDLK_KP_4 || keycode == SDLK_LEFT) {
      return COMMAND_LEFT;
    } else if (keycode == SDLK_KP_5) {
      return COMMAND_CENTER;
    } else if (keycode == SDLK_KP_6 || keycode == SDLK_RIGHT) {
      return COMMAND_RIGHT;
    } else if (keycode == SDLK_KP_2 || keycode == SDLK_DOWN) {
      return COMMAND_DOWN;
    } else if (keycode == SDLK_SPACE) {
      return COMMAND_JUMP;
    } else if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER) {
      return COMMAND_ENTER;
    } else if (keycode == SDLK_c) {
      return COMMAND_CONVERT;
    } else if (keycode == SDLK_q) {
      return COMMAND_QUIT;
    }
  }
  return COMMAND_NONE;
}

/**
 * Asserts whether or not a character is a valid input character.
 *
 * For simplicity, the user should only be able to enter letters and numbers.
 */
int is_valid_input_character(char c) { return isalnum(c); }

/**
 * Prints a string starting from (x, y) but limits to its first limit
 * characters.
 */
static void print_limited(const int x, const int y, const char *string,
                          const size_t limit, SDL_Renderer *renderer) {
  const size_t string_length = strlen(string);
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
    print(x, y, string, COLOR_PAIR_DEFAULT, renderer);
    return;
  }
  /* String is longer than the limit. */
  /* Write the ellipsis if we need to. */
  if (limit >= MINIMUM_STRING_SIZE_FOR_ELLIPSIS) {
    print(x, y, ELLIPSIS_STRING, COLOR_PAIR_DEFAULT, renderer);
  }
  /* Write the tail of the input string. */
  string += string_length - limit + ELLIPSIS_LENGTH;
  print(x + ELLIPSIS_LENGTH, y, string, COLOR_PAIR_DEFAULT, renderer);
}

/**
 * Reads a string from the user of up to size characters (including NUL).
 *
 * The string will be echoed after the prompt, which starts at (x, y).
 */
Code read_string(const int x, const int y, const char *prompt,
                 char *destination, const size_t size, SDL_Renderer *renderer) {
  const int buffer_x = x + strlen(prompt) + 1;
  const int buffer_view_limit = get_columns() - PADDING - buffer_x;
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
      print(x, y, prompt, COLOR_PAIR_DEFAULT, renderer);
      if (written == 0) {
        /* We must write a single space, or SDL will not render anything. */
        print(buffer_x, y, " ", COLOR_PAIR_DEFAULT, renderer);
      } else {
        /*
         * Must care about how much we write, PADDING should be respected.
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

/**
 * Reads the next command that needs to be processed.
 *
 * This is the last pending command.
 *
 * This function consumes the whole input buffer and returns either
 * COMMAND_NONE (if no other Command could be produced by what was in the input
 * buffer) or the last Command different than COMMAND_NONE that could be
 * produced by what was in the input buffer.
 */
Command read_next_command(void) {
  Command last_valid_command = COMMAND_NONE;
  Command current;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    current = command_from_event(event);
    if (current != COMMAND_NONE) {
      last_valid_command = current;
    }
  }
  return last_valid_command;
}

/**
 * Waits for the next command, blocking indefinitely.
 */
Command wait_for_next_command(void) {
  Command command;
  SDL_Event event;
  int got_command = 0;
  while (!got_command) {
    if (SDL_WaitEvent(&event)) {
      command = command_from_event(event);
      got_command = command != COMMAND_NONE;
    }
  }
  return command;
}

/**
 * Waits for any user input, blocking indefinitely.
 */
Code wait_for_input(void) {
  SDL_Event event;
  while (1) {
    if (SDL_WaitEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return CODE_QUIT;
      }
      if (event.type == SDL_KEYDOWN) {
        return CODE_OK;
      }
    } else {
      /* WaitEvent returns 0 to indicate errors. */
      return CODE_ERROR;
    }
  }
}
