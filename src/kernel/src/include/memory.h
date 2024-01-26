#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <boot.h>

#define LONG_MASK (sizeof(unsigned long) - 1)
typedef struct {
    uint8_t status;
    uint32_t size;
} alloc_t;

void *__memcpy(void *dest, const void *src, size_t n);
void __memset(void *_dst, int val, uint32_t len);
char *dumbass_malloc(size_t size);
void __free(void *mem);
uint64_t get_memory_size(Memory_Map_Descriptor *mmap, uint64_t mmap_entries, uint64_t mmap_desc_size);
void m_memset(void *start, uint8_t val, uint64_t num);
#endif