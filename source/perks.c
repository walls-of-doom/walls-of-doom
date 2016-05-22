#include "perks.h"

#include "random.h"

char *get_perk_symbol(void) {
    return "X";
}

Perk get_random_perk(void) {
    return random_integer(0, PERK_COUNT - 1);
}
