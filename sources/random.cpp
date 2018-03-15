#include "random.hpp"
#include "constants.hpp"
#include "data.hpp"
#include "text.hpp"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXIMUM_WORD_SIZE 32

/* These state variables must be initialized so that they are not all zero. */
static unsigned long x;
static unsigned long y;
static unsigned long z;
static unsigned long w;

static unsigned long xorshift128(void) {
  unsigned long t = x;
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

void seed_random(void) {
  /* If tloc is a null pointer, no value is stored. */
  x = (long)time(NULL);
}

/**
 * Returns the next power of two bigger than the provided number.
 */
unsigned long find_next_power_of_two(unsigned long number) {
  unsigned long result = 1;
  while (number) {
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
  unsigned long range;
  unsigned long next_power_of_two;
  unsigned long value;
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
 * Copies the first word of a random line of the file to the destination.
 */
void random_word(char *destination, const char *filename) {
  int read = '\0';
  int chosen_line;
  int current_line;
  const int line_count = file_line_count(filename);
  FILE *file;
  if (line_count > 0) {
    chosen_line = random_integer(0, line_count - 1);
    file = fopen(filename, "r");
    if (file) {
      current_line = 0;
      while (current_line != chosen_line && read != EOF) {
        read = fgetc(file);
        if (read == '\n') {
          current_line++;
        }
      }
      /* Got to the line we want to copy. */
      while ((read = fgetc(file)) != EOF) {
        if (isspace((char)read)) {
          break;
        }
        *destination++ = (char)read;
      }
      fclose(file);
    }
  }
  *destination = '\0';
}

/**
 * Writes a pseudorandom name to the destination.
 *
 * The destination should have at least 2 * MAXIMUM_WORD_SIZE bytes.
 */
void random_name(char *destination) {
  char buffer[MAXIMUM_WORD_SIZE];
  size_t first_word_size;
  random_word(buffer, ADJECTIVES_FILE_PATH);
  first_word_size = strlen(buffer);
  buffer[0] = toupper(buffer[0]);
  copy_string(destination, buffer, MAXIMUM_WORD_SIZE);
  random_word(buffer, NOUNS_FILE_PATH);
  buffer[0] = toupper(buffer[0]);
  copy_string(destination + first_word_size, buffer, MAXIMUM_WORD_SIZE);
}
