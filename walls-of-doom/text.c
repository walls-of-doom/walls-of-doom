#include "text.h"

#include <ctype.h>
#include <stdlib.h>

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
