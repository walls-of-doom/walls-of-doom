#include "perk.hpp"
#include "logger.hpp"
#include "random.hpp"

Perk get_random_perk() {
  return static_cast<Perk>(random_integer(0, PERK_COUNT - 1));
}

bool is_bonus_perk(Perk perk) {
  return perk == PERK_BONUS_EXTRA_POINTS || perk == PERK_BONUS_EXTRA_LIFE;
}

bool is_curse_perk(Perk perk) {
  const auto is_accelerate_platforms = perk == PERK_CURSE_ACCELERATE_PLATFORMS;
  const auto is_reverse_platforms = perk == PERK_CURSE_REVERSE_PLATFORMS;
  return is_accelerate_platforms || is_reverse_platforms;
}

std::string get_perk_name(Perk perk) {
  if (perk == PERK_POWER_INVINCIBILITY) {
    return "Invincibility";
  }
  if (perk == PERK_POWER_LEVITATION) {
    return "Levitation";
  }
  if (perk == PERK_POWER_LOW_GRAVITY) {
    return "Low Gravity";
  }
  if (perk == PERK_POWER_SUPER_JUMP) {
    return "Super Jump";
  }
  if (perk == PERK_POWER_TIME_STOP) {
    return "Time Stop";
  }
  if (perk == PERK_CURSE_ACCELERATE_PLATFORMS) {
    return "Accelerate Platforms";
  }
  if (perk == PERK_CURSE_REVERSE_PLATFORMS) {
    return "Reverse Platforms";
  } else if (perk == PERK_BONUS_EXTRA_POINTS) {
    return "Extra Points";
  } else if (perk == PERK_BONUS_EXTRA_LIFE) {
    return "Extra Life";
  } else {
    log_message("Tried to get the name of an unnamed Perk!");
    return "Unnamed Perk";
  }
}
