#ifndef HEAP_H
#define HEAP_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct __heap_block_t {
    struct __heap_block_t *next;
    uint32_t size;
    uint32_t used;
    uint32_t bsize;
    uint32_t lfb;
} heap_block_t;

typedef struct __heap_bm_t {
    heap_block_t *fblock;
} heap_bm_t;
void heap_init(heap_bm_t *heap);
int heap_add_block(heap_bm_t *heap, uintptr_t addr, uint32_t size, uint32_t bsize);
static uint8_t heap_get_nid(uint8_t a, uint8_t b);
void *heap_malloc(heap_bm_t *heap, uint32_t size);
void heap_free(heap_bm_t *heap, void *ptr);

#endif