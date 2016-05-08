#include "math.h"

int normalize(const int value) {
    if (value < 0) {
        return -1;
    } else if (value == 0) {
        return 0;
    } else {
        return 1;
    }
}
