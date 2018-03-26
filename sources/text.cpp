#include "text.hpp"
#include <cctype>
#include <cstdlib>
#include <cstring>

/**
 * Copy from the source string to the destination string, using at most size
 * characters.
 *
 * NUL terminates the destination string, unless size == 0.
 *
 * This function behaves similarly to strlcpy. However, because the latter may
 * not be available on all platforms, copy_string is used.
 */
size_t copy_string(char *destination, const char *source, const size_t size) {
  char *d = destination;
  const char *s = source;
  size_t n = size;

  while (n > 0) {
    *d = *s;
    if (*s == '\0') {
      break;
    }
    d++;
    s++;
    n--;
  }
  /* Not enough space in destination, add NUL if possible. */
  if (n == 0) {
    if (size != 0) {
      *d = '\0';
    }
  }
  while (*s != 0) {
    s++;
  }
  /* Here s points to the first occurrence of '\0' in source. */
  return s - source;
}

/**
 * Copy from the source string to the destination string, up to the specified
 * character, using at most size characters.
 *
 * NUL terminates the destination string, unless size == 0.
 */
size_t copy_string_up_to(char *destination, const char *begin, const char *end, const size_t size) {
  char *d = destination;
  const char *s = begin;
  /* Copy only size - 1 characters in order to NUL terminate. */
  size_t n = size - 1;

  if (size == 0) {
    return 0;
  }

  while (s != end && n != 0) {
    *d++ = *s++;
    n--;
  }
  *d = '\0';

  if (s == end) {
    return end - begin + 1;
  }
  return size;
}

bool string_equals(const char *a, const char *b) { return strcmp(a, b) == 0; }

/**
 * Trims a string by removing all leading and trailing spaces.
 */
void trim_string(char *string) {
  char *write = string;
  char *read = string;
  /* Find the first not space. */
  while (*read != '\0' && (isspace(*read) != 0)) {
    read++;
  }
  /* Copy everything from the first not space up to the end. */
  while (*read != '\0') {
    *write++ = *read++;
  }
  /* Now proceed to trim the end of the string. */
  /* read points to NUL here. */
  /* If we can march back. */
  if (read != string) {
    /* Point to the last character. */
    read--;
    while ((isspace(*read) != 0) || read > write) {
      *read = '\0';
      if (read == string) {
        /* Do not write before the start of the string. */
        break;
      }
      read--;
    }
  }
}

/**
 * Wraps the input string at the right margin respecting a limit of COLUMNS.
 *
 * If a line cannot be wrapped because it has a word with more characters than
 * COLUMNS, the line is untouched.
 */
void wrap_at_right_margin(char *string, const size_t columns) {
  const size_t string_length = strlen(string);
  /* BEGIN should always point to the FIRST character of a line. */
  size_t begin = 0;
  /* We point to the maximum point where a line should end, then backtrack. */
  size_t end = 0;
  end = begin + columns;
  while (end < string_length) {
    while (begin != end && (isspace(string[end]) == 0)) {
      end--;
    }
    if (begin == end) {
      /* There are no spaces in this line, so we can't break it anywhere. */
      /* We work around this by finding the next space on the string. */
      while (end < string_length && (isspace(string[end]) == 0)) {
        end++;
      }
    }
    if (end < string_length) {
      string[end] = '\n';
    }
    begin = end + 1;
    end = begin + columns;
  }
}
