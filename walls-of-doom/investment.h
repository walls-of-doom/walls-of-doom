#ifndef INVESTMENT_H
#define INVESTMENT_H

#include "score.h"

typedef struct Investment {
  struct Investment *next;
  unsigned long end;
  Score amount;
} Investment;

#endif
