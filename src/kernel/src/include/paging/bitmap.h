#ifndef BITMAP_H
#define BITMAP_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct __bitmap_t {
    size_t size;
    uint8_t *buffer;
} bitmap_t;

bool bitmap_get(bitmap_t *this, uint64_t index);
void bitmap_set(bitmap_t *this, uint64_t index, bool value);

#endif