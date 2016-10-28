#include "investment.h"
#include "random.h"
#include "settings.h"

int end_investment(const Investment investment) {
  const int maximum_return = get_investment_maximum_factor();
  const int minimum_return = get_investment_minimum_factor();
  return random_integer(minimum_return, maximum_return) * investment.amount /
         100;
}
