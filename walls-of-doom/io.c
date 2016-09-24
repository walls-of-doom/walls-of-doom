#include "io.h"

#include "clock.h"
#include "constants.h"
#include "game.h"
#include "logger.h"
#include "math.h"
#include "physics.h"
#include "player.h"
#include "rest.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define ELLIPSIS_STRING "..."
#define ELLIPSIS_LENGTH (strlen(ELLIPSIS_STRING))
#define MINIMUM_STRING_SIZE_FOR_ELLIPSIS (2 * ELLIPSIS_LENGTH)

#define IMG_FLAGS IMG_INIT_PNG

static TTF_Font *global_monospaced_font = NULL;
static int global_monospaced_font_width = 0;
static int global_monospaced_font_height = 0;

void clean(SDL_Renderer *renderer) { SDL_RenderClear(renderer); }

void present(SDL_Renderer *renderer) { SDL_RenderPresent(renderer); }

/**
 *
 * Initialization and finalization functions.
 *
 */

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
  font = TTF_OpenFont(MONOSPACED_FONT_PATH, MONOSPACED_FONT_SIZE);
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
  Uint32 flags = 0;
  return SDL_CreateWindow(title, x, y, width, height, flags);
}

int set_window_title_and_icon(SDL_Window *window) {
  SDL_SetWindowTitle(window, GAME_NAME);
  SDL_Surface *icon_surface = IMG_Load(ICON_PATH);
  if (icon_surface == NULL) {
    log_message("Failed to load the window icon");
    return 1;
  }
  SDL_SetWindowIcon(window, icon_surface);
  SDL_FreeSurface(icon_surface);
  return 0;
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
  SDL_Renderer *rendererSurface = NULL;
  int width = 1;
  int height = 1;
  initialize_logger();
  /* Initialize SDL. */
  if (SDL_Init(SDL_INIT_VIDEO)) {
    sprintf(log_buffer, "SDL initialization error: %s", SDL_GetError());
    log_message(log_buffer);
    return 1;
  }
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
  width = global_monospaced_font_width * COLUMNS;
  height = global_monospaced_font_height * LINES;
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
  SDL_SetRenderDrawColor(*renderer, 0x33, 0x33, 0x33, 0xFF);
  clean(*renderer);
  return 0;
}

/**
 * Finalizes the global fonts.
 *
 * Returns 0 in case of success.
 */
static int finalize_fonts(void) {
  if (global_monospaced_font != NULL) {
    TTF_CloseFont(global_monospaced_font);
    global_monospaced_font = NULL;
  }
  return 0;
}

/**
 * Finalizes the acquired resources.
 *
 * Should only be called once, right before exiting.
 *
 * Returns 0 in case of success.
 */
int finalize(SDL_Window **window, SDL_Renderer **renderer) {
  finalize_fonts();
  SDL_DestroyWindow(*window);
  *window = NULL;
  if (TTF_WasInit()) {
    TTF_Quit();
  }
  SDL_Quit();
  finalize_logger();
  return 0;
}

/**
 * Initializes the color schemes used to render the game.
 */
int initialize_color_schemes(void) {
  /* Nothing to do for now. */
  return 0;
}

/**
 *
 * Colors.
 *
 */
static SDL_Color get_light_grey(void) {
  SDL_Color grey;
  /* #D3D7CF */
  grey.r = 0xD3;
  grey.g = 0xD7;
  grey.b = 0xCF;
  grey.a = 0xFF;
  return grey;
}

static SDL_Color get_text_color(void) {
  SDL_Color text_color = get_light_grey();
  return text_color;
}

/**
 * Returns a pointer to the start of the text of the string.
 *
 * This is either the first character which is not a space or '\0'.
 */
char *find_start_of_text(char *string) {
  while (*string != '\0' && isspace(*string)) {
    string++;
  }
  return string;
}

/**
 * Returns a pointer to the end of the text of the string.
 *
 * This is either the first trailing space or '\0'.
 */
char *find_end_of_text(char *string) {
  char *last_not_space = string;
  while (*string != '\0') {
    if (!isspace(*string)) {
      last_not_space = string;
    }
    string++;
  }
  if (*last_not_space != '\0') {
    last_not_space++;
  }
  return last_not_space;
}

/**
 * Trims a string by removing all leading and trailing spaces.
 */
void trim_string(char *string) {
  int copying = 0;
  char *write = string;
  char *read = string;
  /* Find the first not space. */
  while (*read != '\0' && isspace(*read)) {
    read++;
  }
  /* Copy everthing from the first not space up to the end. */
  while (*read != '\0') {
    *write++ = *read++;
  }
  /* Now proceed to trim the end of the string. */
  /* read points to NUL here. */
  if (read != string) { /* If we can march back. */
    read--;             /* Point to the last character. */
    while (isspace(*read) || read > write) {
      *read = '\0';
      if (read == string) {
        break; /* Do not write before the start of the string. */
      }
      read--;
    }
  }
}

/**
 * Evaluates whether or not a Player name is a valid name.
 *
 * A name is considered to be valid if it has at least two characters after
 * being trimmed.
 */
int is_valid_player_name(const char *player_name) {
  char buffer[MAXIMUM_PLAYER_NAME_SIZE];
  safe_strcpy(buffer, player_name, MAXIMUM_PLAYER_NAME_SIZE);
  trim_string(buffer);
  return strlen(buffer) >= 2;
}

void read_player_name(char *destination, const size_t maximum_size,
                      SDL_Renderer *renderer) {
  int x;
  int y;
  int error = 0;
  int valid_name = 0;
  const char message[] = "Name your character: ";
  char log_buffer[MAXIMUM_STRING_SIZE];
  /* While there is not a read error or a valid name. */
  while (!error && !valid_name) {
    x = PADDING;
    y = LINES / 2;
    error = read_string(x, y, message, destination, maximum_size, renderer);
    if (error) {
      log_message("Failed to read player name");
      /* Cope with it by providing a name for the player. */
      safe_strcpy(destination, FALLBACK_PLAYER_NAME, maximum_size);
    } else {
      sprintf(log_buffer, "Read '%s' from the user", destination);
      log_message(log_buffer);
      /* Trim the name the user entered. */
      trim_string(destination);
      sprintf(log_buffer, "Trimmed the input to '%s'", destination);
      log_message(log_buffer);
      valid_name = is_valid_player_name(destination);
    }
  }
}

/**
 * Prints the provided string on the screen starting at (x, y).
 *
 * Returns 0 in case of success.
 */
int print(const int x, const int y, const char *string,
          SDL_Renderer *renderer) {
  TTF_Font *font = global_monospaced_font;
  SDL_Surface *surface;
  SDL_Texture *texture;
  SDL_Rect position;
  position.x = global_monospaced_font_width * x;
  position.y = global_monospaced_font_height * y;
  /* Validate that x and y are nonnegative. */
  if (x < 0 || y < 0) {
    return 1;
  }
  surface = TTF_RenderText_Blended(font, string, get_text_color());
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

/**
 * Prints the provided string centered on the screen at the provided line.
 */
void print_centered(const int y, const char *string, SDL_Renderer *renderer) {
  const int x = (COLUMNS - strlen(string)) / 2;
  print(x, y, string, renderer);
}

/**
 * Replaces all newlines, carriage returns, and tabs by the ASCII space.
 */
void normalize_whitespaces(char *string) {
  char c;
  size_t i;
  for (i = 0; string[i] != '\0'; i++) {
    c = string[i];
    if (c == '\n' || c == '\t') {
      string[i] = ' ';
    }
  }
}

void wrap_at_right_margin(char *string, const size_t columns) {
  size_t last_line_start = 0;
  size_t string_length = strlen(string);
  while (string_length - last_line_start > columns) {
    size_t next_line_start = last_line_start + columns;
    while (string[next_line_start] != ' ') {
      next_line_start--;
      if (next_line_start == last_line_start) {
        /* There are no spaces in this line, so we can't do anything. */
        /* Abort, simply. */
        break;
      }
      next_line_start--;
    }
    string[next_line_start] = '\n';
    last_line_start = next_line_start;
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
  size_t lines_copied = 0;
  char line[COLUMNS + 1];
  char *cursor;
  int width = COLUMNS - PADDING;
  int line_count;
  normalize_whitespaces(string);
  wrap_at_right_margin(string, width);
  line_count = count_lines(string);
  clean(renderer);
  /* Print each line. */
  cursor = string;
  while (*cursor != '\0') {
    cursor = copy_first_line(cursor, line);
    print(PADDING, (LINES - line_count) / 2 + lines_copied, line, renderer);
    lines_copied++;
  }
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

  const int columns_per_string = COLUMNS / TOP_BAR_STRING_COUNT;

  char buffer[COLUMNS + 1];
  memset(buffer, ' ', COLUMNS);
  buffer[COLUMNS] = '\0';

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
      after_x = COLUMNS;
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
      safe_strcpy(buffer + x, strings[i], COLUMNS + 1 - x);
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
  print(0, 0, buffer, renderer);
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
void draw_bottom_bar(SDL_Renderer *renderer) {
  char buffer[COLUMNS + 1];
  memset(buffer, ' ', COLUMNS);
  buffer[COLUMNS] = '\0';
  print(0, LINES - 1, buffer, renderer);
}

/**
 * Draws the borders of the screen.
 */
void draw_borders(SDL_Renderer *renderer) {
  int i;
  char buffer[COLUMNS + 1];
  memset(buffer, '+', COLUMNS);
  buffer[COLUMNS] = '\0';
  print(0, 1, buffer, renderer);
  print(0, LINES - 1, buffer, renderer);
  memset(buffer + 1, ' ', COLUMNS - 2);
  for (i = 1; i < LINES - 1; i++) {
    print(0, i, buffer, renderer);
  }
}

int draw_platforms(const Platform *platforms, const size_t platform_count,
                   const BoundingBox *const box, SDL_Renderer *renderer) {
  int y;
  int min_x;
  int max_x;
  size_t i;
  /* We make the assumption that the biggest box is COLUMNS wide. */
  char buffer[COLUMNS + 1];
  memset(buffer, '=', COLUMNS + 1);
  buffer[COLUMNS] = '\0';
  for (i = 0; i < platform_count; i++) {
    y = platforms[i].y;
    min_x = platforms[i].x;
    max_x = platforms[i].x + platforms[i].width - 1;
    if (y >= box->min_y && y <= box->max_y) {
      if (min_x <= box->max_x && max_x >= box->min_x) {
        min_x = max(box->min_x, min_x);
        max_x = min(box->max_x, max_x);
        print(min_x, y, buffer + COLUMNS - (max_x - min_x + 1), renderer);
      }
    }
  }
  return 0;
}

int has_active_perk(const Game *const game) { return game->perk != PERK_NONE; }

ColorScheme get_perk_color(Perk perk) {
  if (perk == PERK_POWER_INVINCIBILITY) {
    return COLOR_INVINCIBILITY;
  } else if (perk == PERK_POWER_LEVITATION) {
    return COLOR_LEVITATION;
  } else if (perk == PERK_POWER_LOW_GRAVITY) {
    return COLOR_LOW_GRAVITY;
  } else if (perk == PERK_POWER_SUPER_JUMP) {
    return COLOR_SUPER_JUMP;
  } else if (perk == PERK_POWER_TIME_STOP) {
    return COLOR_TIME_STOP;
  } else if (perk == PERK_BONUS_EXTRA_POINTS) {
    return COLOR_EXTRA_POINTS;
  } else if (perk == PERK_BONUS_EXTRA_LIFE) {
    return COLOR_EXTRA_LIFE;
  } else {
    char buffer[MAXIMUM_STRING_SIZE];
    sprintf(buffer, "get_perk_color called with invalid value: %d", perk);
    log_message(buffer);
    return COLOR_PLAYER;
  }
}

int draw_perk(const Game *const game, SDL_Renderer *renderer) {
  if (has_active_perk(game)) {
    print(game->perk_x, game->perk_y, get_perk_symbol(), renderer);
  }
  return 0;
}

int draw_player(const Player *const player, SDL_Renderer *renderer) {
  print(player->x, player->y, PLAYER_SYMBOL, renderer);
  return 0;
}

/**
 * Draws a full game to the screen.
 */
int draw_game(const Game *const game, SDL_Renderer *renderer) {
  clean(renderer);
  draw_top_bar(game->player, renderer);
  draw_bottom_bar(renderer);
  draw_borders(renderer);
  draw_platforms(game->platforms, game->platform_count, game->box, renderer);
  draw_perk(game, renderer);
  draw_player(game->player, renderer);
  present(renderer);
  return 0;
}

void print_game_result(const char *name, const unsigned int score,
                       const int position, SDL_Renderer *renderer) {
  char first_line[MAXIMUM_STRING_SIZE];
  char second_line[MAXIMUM_STRING_SIZE];
  sprintf(first_line, "%s died after making %d points.", name, score);
  if (position > 0) {
    sprintf(second_line, "%s got to position %d!", name, position);
  } else {
    sprintf(second_line, "%s didn't make it to the top scores.", name);
  }
  print_centered(LINES / 2 - 1, first_line, renderer);
  print_centered(LINES / 2 + 1, second_line, renderer);
}

/**
 * Returns a BoundingBox that represents the playable area after removing bars
 * and margins.
 */
BoundingBox bounding_box_from_screen(void) {
  BoundingBox box;
  box.min_x = 1;
  box.min_y = 2; /* Top bar. */
  box.max_x = COLUMNS - 2;
  box.max_y = LINES - 3; /* Bottom bar. */
  return box;
}

/**
 * Returns the Command value corresponding to the provided input code.
 */
Command command_from_event(const SDL_Event event) {
  SDL_Keycode keycode;
  if (event.type == SDL_QUIT) {
    return COMMAND_QUIT;
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
                          const int limit, SDL_Renderer *renderer) {
  /* No-op. */
  if (limit < 1) {
    return;
  }
  const size_t string_length = strlen(string);
  /*
   * As only two calls to print suffice to solve this problem for any possible
   * input size, we avoid using a dynamically allocated buffer to prepare the
   * output.
   */
  /* String length is less than the limit. */
  if (string_length < limit) {
    print(x, y, string, renderer);
    return;
  }
  /* String is longer than the limit. */
  /* Write the ellipsis if we need to. */
  if (limit >= MINIMUM_STRING_SIZE_FOR_ELLIPSIS) {
    print(x, y, ELLIPSIS_STRING, renderer);
  }
  /* Write the tail of the input string. */
  string += string_length - limit + ELLIPSIS_LENGTH;
  print(x + ELLIPSIS_LENGTH, y, string, renderer);
}

/**
 * Reads a string from the user of up to size characters (including NUL).
 *
 * The string will be echoed after the prompt, which starts at (x, y).
 *
 * Returns 0 if successful.
 * Returns 1 if the user tried to quit.
 */
int read_string(const int x, const int y, const char *prompt, char *destination,
                const size_t size, SDL_Renderer *renderer) {
  const int buffer_x = x + strlen(prompt) + 1;
  const int buffer_view_limit = COLUMNS - PADDING - buffer_x;
  int is_done = 0;
  int should_rerender = 1;
  /* The x coordinate of the user input buffer. */
  size_t written = 0;
  char *write = destination;
  char character = '\0';
  SDL_Event event;
  /* Make sure that we always have a valid string for printing. */
  /* Throughout the loop, the write pointer always points to a '\0'. */
  *destination = '\0';
  /* Start listening for text input. */
  SDL_StartTextInput();
  while (!is_done) {
    if (should_rerender) {
      clean(renderer);
      print(x, y, prompt, renderer);
      if (written == 0) {
        /* We must write a single space, or SDL will not render anything. */
        print(buffer_x, y, " ", renderer);
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
    if (SDL_PollEvent(&event)) {
      /* Check for user quit and return 1. */
      /* This is OK because the destination string is always a valid C string.
       */
      if (event.type == SDL_QUIT) {
        return 1;
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
  return 0;
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
  Command command = COMMAND_NONE;
  while (command == COMMAND_NONE) {
    rest_for_second_fraction(FPS);
    command = read_next_command();
  }
  return command;
}

/**
 * Waits for any user input, blocking indefinitely.
 */
Code wait_for_input(void) {
  SDL_Event event;
  while (1) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        return CODE_QUIT;
      }
      if (event.type == SDL_KEYDOWN) {
        return CODE_OK;
      }
    }
  }
}
