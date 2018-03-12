#ifndef INVESTMENT_H
#define INVESTMENT_H

#include "score.h"
#include <string>

typedef struct Investment {
  struct Investment *next;
  unsigned long end;
  Score amount;
} Investment;

typedef enum InvestmentMode {
  INVESTMENT_MODE_FIXED,
  INVESTMENT_MODE_PROPORTIONAL,
  INVESTMENT_MODE_COUNT
} InvestmentMode;

std::string get_investment_mode_name(InvestmentMode mode);

#endif
