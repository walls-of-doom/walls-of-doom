#include "random.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "text.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>

#define MAXIMUM_WORD_SIZE 32

static const char *const NAME_FILE_PATH = "data/name.txt";

/* These state variables must be initialized so that they are not all zero. */
static U64 x;
static U64 y;
static U64 z;
static U64 w;

static U64 xorshift128() {
  U64 t = x;
  /* Left shif overflow is undefined behavior in C. */
  t ^= t << 11;
  t ^= t >> 8;
  x = y;
  y = z;
  z = w;
  w ^= w >> 19;
  w ^= t;
  return w;
}

void seed_random() {
  /* If tloc is a null pointer, no value is stored. */
  x = static_cast<long>(time(nullptr));
}

/**
 * Returns the next power of two bigger than the provided number.
 */
U64 find_next_power_of_two(U64 number) {
  U64 result = 1;
  while (number != 0u) {
    number >>= 1;
    result <<= 1;
  }
  return result;
}

/**
 * Returns a random number in the range [minimum, maximum].
 *
 * Always returns 0 if maximum < minimum.
 */
int random_integer(const int minimum, const int maximum) {
  /* Range should be a bigger type because the difference may overflow int. */
  U64 range;
  U64 next_power_of_two;
  U64 value;
  if (maximum < minimum) {
    return 0;
  }
  /* This never overflows, even when int and long have the same size. */
  range = maximum - minimum + 1;
  next_power_of_two = find_next_power_of_two(range);
  do {
    value = xorshift128() % next_power_of_two;
  } while (value >= range);
  /*
   * Varies from
   *   minimum + 0 == minimum
   * to
   *   minimum + (maximum - minimum + 1 - 1) == maximum
   */
  return minimum + value;
}

/**
 * Returns the first word of a random line of the file.
 */
static std::string random_word(const std::string &filename) {
  int read = '\0';
  int chosen_line;
  int current_line;
  const int line_count = file_line_count(filename.c_str());
  FILE *file;
  std::string destination;
  if (line_count > 0) {
    chosen_line = random_integer(0, line_count - 1);
    file = fopen(filename.c_str(), "r");
    if (file != nullptr) {
      current_line = 0;
      while (current_line != chosen_line && read != EOF) {
        read = fgetc(file);
        if (read == '\n') {
          current_line++;
        }
      }
      /* Got to the line we want to copy. */
      while ((read = fgetc(file)) != EOF) {
        if (isspace(static_cast<char>(read)) != 0) {
          break;
        }
        destination += static_cast<char>(read);
      }
      fclose(file);
    }
  }
  return destination;
}

static std::string get_stored_name() {
  std::string destination;
  Code code = read_characters(NAME_FILE_PATH, destination);
  if (code != CODE_OK) {
    throw std::runtime_error("Failed to read file.");
  }
  return destination;
}

static bool has_stored_name() {
  try {
    get_stored_name();
    return true;
  } catch (std::exception &exception) {
    return false;
  }
}

static std::string get_random_name() {
  auto a = random_word(ADJECTIVES_FILE_PATH);
  auto b = random_word(NOUNS_FILE_PATH);
  a[0] = toupper(a[0]);
  b[0] = toupper(b[0]);
  return a + b;
}

std::string get_user_name() {
  if (has_stored_name()) {
    return get_stored_name();
  }
  return get_random_name();
}
