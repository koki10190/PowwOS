#include <stdlib.h>

uint32_t rseed = 1;

void seed(uint32_t seed) {
    rseed = seed;
}

uint32_t rand() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;

    x *= 23786259 - rseed;

    uint32_t t;

    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}