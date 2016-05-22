#include "perks.h"

#include "random.h"

char get_perk_symbol(Perk perk) {
    return 'o';
}

Perk get_random_perk() {
    return random_integer(0, PERK_COUNT - 1);
}
