#include "perk.h"
#include "logger.h"
#include "random.h"

Perk get_random_perk(void) { return random_integer(0, PERK_COUNT - 1); }

int is_bonus_perk(const Perk perk) {
  return perk == PERK_BONUS_EXTRA_POINTS || perk == PERK_BONUS_EXTRA_LIFE;
}

int is_curse_perk(const Perk perk) {
  return perk == PERK_CURSE_REVERSE_PLATFORMS;
}

/**
 * Returns the name of the provided Perk.
 *
 * The biggest string this function returns has 13 printable characters.
 */
char *get_perk_name(Perk perk) {
  if (perk == PERK_POWER_INVINCIBILITY) {
    return "Invincibility";
  } else if (perk == PERK_POWER_LEVITATION) {
    return "Levitation";
  } else if (perk == PERK_POWER_LOW_GRAVITY) {
    return "Low Gravity";
  } else if (perk == PERK_POWER_SUPER_JUMP) {
    return "Super Jump";
  } else if (perk == PERK_POWER_TIME_STOP) {
    return "Time Stop";
  } else if (perk == PERK_CURSE_REVERSE_PLATFORMS) {
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
