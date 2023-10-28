#ifndef PAGE_FRAME_ALLOCATOR_H
#define PAGE_FRAME_ALLOCATOR_H

#include <boot.h>
#include <stdint.h>
#include <paging/bitmap.h>
#include <memory.h>

typedef struct __page_frame_allocator_t {
    bitmap_t bitmap;
} page_frame_allocator_t;

extern page_frame_allocator_t global_allocator;

void pg_alloc_read_memory_map(page_frame_allocator_t *this, Memory_Map_Descriptor *mmap, size_t mmap_size, size_t mmap_desc_size);
void pg_alloc_init_bitmap(page_frame_allocator_t *this, size_t bitmap_size, void *buffer_address);
void pg_alloc_free_page(page_frame_allocator_t *this, void *address);
void pg_alloc_lock_page(page_frame_allocator_t *this, void *address);
void *pg_alloc_request_page(page_frame_allocator_t *this);
// private
void pg_alloc_reserve_page(page_frame_allocator_t *this, void *address);
void pg_alloc_unreserve_page(page_frame_allocator_t *this, void *address);
// quantity
void pg_alloc_free_pages(page_frame_allocator_t *this, void *address, uint64_t page_count);
void pg_alloc_lock_pages(page_frame_allocator_t *this, void *address, uint64_t page_count);
void pg_alloc_reserve_pages(page_frame_allocator_t *this, void *address, uint64_t page_count);
void pg_alloc_unreserve_pages(page_frame_allocator_t *this, void *address, uint64_t page_count);
uint64_t get_free_ram();
uint64_t get_used_ram();
uint64_t get_reserved_ram();

#endif