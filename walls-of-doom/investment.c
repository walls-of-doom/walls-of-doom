#include "investment.h"

char *get_investment_mode_name(InvestmentMode mode) {
  if (mode == INVESTMENT_MODE_FIXED) {
    return "FIXED";
  } else if (mode == INVESTMENT_MODE_PROPORTIONAL) {
    return "PROPORTIONAL";
  } else {
    return "\0";
  }
}
