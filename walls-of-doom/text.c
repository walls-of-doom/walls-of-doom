#include "text.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
 * Copy from the source string to the destination string, using at most size
 * characters.
 *
 * NUL terminates the destination string, unless size == 0.
 *
 * This function behaves similarly to strlcpy. However, because the latter may
 * not be available on all platforms, copy_string is used.
 */
size_t copy_string(char *destination, const char *source, size_t size) {
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
  while (*s) {
    s++;
  }
  /* Here s points to the first occurrence of '\0' in source. */
  return s - source;
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
    while (isspace(*read) || read > write) {
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
    while (begin != end && !isspace(string[end])) {
      end--;
    }
    if (begin == end) {
      /* There are no spaces in this line, so we can't break it anywhere. */
      /* We work around this by finding the next space on the string. */
      while (end < string_length && !isspace(string[end])) {
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
