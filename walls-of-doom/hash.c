#include "hash.h"

unsigned long hash_string(const char *string) {
  unsigned long hash = 5381UL;
  while (*string) {
    hash = (33 * hash) ^ *string;
    string++;
  }
  return hash;
}
