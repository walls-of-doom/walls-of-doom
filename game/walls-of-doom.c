#include "io.h"
#include "menu.h"
#include "random.h"

int main(void) {
    initialize();
    seed_random();
    int result = main_menu();
    finalize();
    return result;
}
