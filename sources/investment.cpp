#include "investment.hpp"

std::string get_investment_mode_name(InvestmentMode mode) {
  if (mode == INVESTMENT_MODE_FIXED) {
    return "FIXED";
  }
  if (mode == INVESTMENT_MODE_PROPORTIONAL) {
    return "PROPORTIONAL";
  }
  return "";
}