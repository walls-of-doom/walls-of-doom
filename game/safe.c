#include <stdlib.h>

/*
 * Copy from the source string to the destination string, using at most size
 * characters.
 *
 * NUL terminates the destination string, unless size == 0.
 *
 * This function behaves similarly to strlcpy. However, because the latter may
 * not be available on all platforms, safe_strcpy is used.
 */
size_t safe_strcpy(char *destination, const char *source, size_t size) {
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
