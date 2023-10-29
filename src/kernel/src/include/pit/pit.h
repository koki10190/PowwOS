#ifndef PIT_H
#define PIT_H

#include <stdint.h>

extern double time_since_boot;
extern const uint64_t base_frequency;

void sleepd(double seconds);
void sleep(uint64_t miliseconds);

void set_divisor(uint16_t __divisor);
uint16_t get_frequency();
void set_frequency(uint64_t frequency);
void tick();

#endif