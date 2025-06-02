#include "Arduino.h"

static uint32_t randomSeedValue = 0;

void randomSeed(unsigned long seed) {
    if (seed != 0) {
        randomSeedValue = seed;
        srand(randomSeedValue);
    }
}

long random(long max) {
    if (max <= 0) {
        return 0;
    }
    return rand() % max;
}


long random(long min, long max) {
    if (min >= max) {
        return min;
    }
    return min + random(max - min);
}
