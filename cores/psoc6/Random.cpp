#include "Arduino.h"
#include "cyhal_trng.h"

void randomSeed(unsigned long seed) {
}

long random(long max) {
    if (max <= 0) {
        return 0;
    }

    static bool initializedTRNG = false;
    static cyhal_trng_t trng_obj;

    if (!initializedTRNG) {
        if (cyhal_trng_init(&trng_obj) != CY_RSLT_SUCCESS) {
            return 0;
        }
        initializedTRNG = true;
    }
    uint32_t rnd_num = cyhal_trng_generate(&trng_obj);

    return rnd_num % max;
}

long random(long min, long max) {
    if (min >= max) {
        return min;
    }
    return min + random(max - min);
}
