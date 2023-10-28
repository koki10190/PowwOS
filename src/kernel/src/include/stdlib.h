#ifndef STDLIB_H
#define STDLIB_H
#include <stdint.h>

extern uint32_t rseed;
void seed(uint32_t seed);
uint32_t rand();

#endif