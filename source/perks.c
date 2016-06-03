#include "perks.h"

#include "logger.h"
#include "random.h"

char *get_perk_symbol(void) {
    return "X";
}

Perk get_random_perk(void) {
    return random_integer(0, PERK_COUNT - 1);
}

int is_bonus_perk(Perk perk) {
    return perk == PERK_BONUS_EXTRA_POINTS || perk == PERK_BONUS_EXTRA_LIFE;
}
