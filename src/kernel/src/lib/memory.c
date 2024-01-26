#include <memory.h>
#include <uart.h>
#include <string.h>

void *memcpy(void *dest, const void *src, size_t n) {
    uint32_t num_dwords = n / 4;
    uint32_t num_bytes = n % 4;
    uint32_t *dest32 = (uint32_t *)dest;
    uint32_t *src32 = (uint32_t *)src;
    uint8_t *dest8 = ((uint8_t *)dest) + num_dwords * 4;
    uint8_t *src8 = ((uint8_t *)src) + num_dwords * 4;
    uint32_t i;

    for (i = 0; i < num_dwords; i++) {
        dest32[i] = src32[i];
    }
    for (i = 0; i < num_bytes; i++) {
        dest8[i] = src8[i];
    }
    return dest;
}

void __memset(void *_dst, int val, uint32_t len) {
    unsigned char *dst = _dst;
    unsigned long *ldst;
    unsigned long lval = (val & 0xFF) * (-1ul / 255); // the multiplier becomes 0x0101... of the same length as long

    if (len >= 16) // optimize only if it's worth it (limit is a guess)
    {
        while ((uintptr_t)dst & LONG_MASK) {
            *dst++ = val;
            len--;
        }
        ldst = (void *)dst;
        while (len > sizeof(long)) {
            *ldst++ = lval;
            len -= sizeof(long);
        }
        dst = (void *)ldst;
    }
    while (len--)
        *dst++ = val;
}

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

char *dumbass_malloc(size_t size) {
    if (!size)
        return 0;

    /* Loop through blocks and find a block sized the same or bigger */
    uint8_t *mem = (uint8_t *)heap_begin;
    while ((uint32_t)mem < last_alloc) {
        alloc_t *a = (alloc_t *)mem;
        /* If the alloc has no size, we have reaced the end of allocation */
        // mprint("mem=0x%x a={.status=%d, .size=%d}\n", mem, a->status, a->size);
        if (!a->size)
            goto nalloc;
        /* If the alloc has a status of 1 (allocated), then add its size
         * and the sizeof alloc_t to the memory and continue looking.
         */
        if (a->status) {
            mem += a->size;
            mem += sizeof(alloc_t);
            mem += 4;
            continue;
        }
        /* If the is not allocated, and its size is bigger or equal to the
         * requested size, then adjust its size, set status and return the location.
         */
        if (a->size >= size) {
            /* Set to allocated */
            a->status = 1;

            __memset(mem + sizeof(alloc_t), 0, size);
            memory_used += size + sizeof(alloc_t);
            return (char *)(mem + sizeof(alloc_t));
        }
        /* If it isn't allocated, but the size is not good, then
         * add its size and the sizeof alloc_t to the pointer and
         * continue;
         */
        mem += a->size;
        mem += sizeof(alloc_t);
        mem += 4;
    }

nalloc:;
    if (last_alloc + size + sizeof(alloc_t) >= heap_end) {
        return NULL;
    }
    alloc_t *alloc = (alloc_t *)last_alloc;
    alloc->status = 1;
    alloc->size = size;

    last_alloc += size;
    last_alloc += sizeof(alloc_t);
    last_alloc += 4;
    memory_used += size + 4 + sizeof(alloc_t);
    __memset((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
    return (char *)((uint32_t)alloc + sizeof(alloc_t));
    /*
            char* ret = (char*)last_alloc;
            last_alloc += size;
            if(last_alloc >= heap_end)
            {
                    set_task(0);
                    panic("Cannot allocate %d bytes! Out of memory.\n", size);
            }
            mprint("Allocated %d bytes from 0x%x to 0x%x\n", size, ret, last_alloc);
            return ret;*/
}

void __free(void *mem) {
    alloc_t *alloc = (mem - sizeof(alloc_t));
    memory_used -= alloc->size + sizeof(alloc_t);
    alloc->status = 0;
}

void *__memcpy(void *dest, const void *src, size_t n) {
    uint32_t num_dwords = n / 4;
    uint32_t num_bytes = n % 4;
    uint32_t *dest32 = (uint32_t *)dest;
    uint32_t *src32 = (uint32_t *)src;
    uint8_t *dest8 = ((uint8_t *)dest) + num_dwords * 4;
    uint8_t *src8 = ((uint8_t *)src) + num_dwords * 4;
    uint32_t i;

    for (i = 0; i < num_dwords; i++) {
        dest32[i] = src32[i];
    }
    for (i = 0; i < num_bytes; i++) {
        dest8[i] = src8[i];
    }
    return dest;
}

uint64_t get_memory_size(Memory_Map_Descriptor *mmap, uint64_t mmap_entries, uint64_t mmap_desc_size) {
    static uint64_t memory_size_bytes = 0;
    if (memory_size_bytes > 0)
        return memory_size_bytes;

    for (int i = 0; i < mmap_entries; i++) {
        Memory_Map_Descriptor *desc = (Memory_Map_Descriptor *)((uint64_t)mmap + (i * mmap_desc_size));
        memory_size_bytes += desc->count * 4096;
        // uart_puts(__itoa(memory_size_bytes));
        // uart_puts("\n");
    }

    return memory_size_bytes;
}

void m_memset(void *start, uint8_t val, uint64_t num) {
    for (uint64_t i = 0; i < num; i++) {
        *(uint8_t *)((uint64_t)start + 1) = val;
    }
}