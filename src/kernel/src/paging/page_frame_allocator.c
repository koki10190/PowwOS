#include <paging/page_frame_allocator.h>
#include <uart.h>
#include <string.h>
uint64_t free_memory;
uint64_t reserved_memory;
uint64_t used_memory;
bool initialized = false;

void pg_alloc_read_memory_map(page_frame_allocator_t *this, Memory_Map_Descriptor *mmap, size_t mmap_size, size_t mmap_desc_size) {
    if (initialized)
        return;
    initialized = true;

    uint64_t mmap_entries = mmap_size / mmap_desc_size;
    void *largest_free_memory_seg = NULL;
    size_t largest_free_memory_segsize = 0;

    for (int i = 0; i < mmap_entries; i++) {
        Memory_Map_Descriptor *desc = (Memory_Map_Descriptor *)((uint64_t)mmap + (i * mmap_desc_size));
        if (desc->type == 7) {
            if (desc->count * 4096 > largest_free_memory_segsize) {
                largest_free_memory_seg = (void *)desc->physical_start;
                largest_free_memory_segsize = desc->count * 4096;
            }
        }
    }

    uint64_t memory_size = get_memory_size(mmap, mmap_entries, mmap_desc_size);
    free_memory = memory_size;
    uint64_t bitmap_size = memory_size / 4096 / 8 + 1;

    pg_alloc_init_bitmap(this, bitmap_size, largest_free_memory_seg);
    pg_alloc_lock_pages(this, &this->bitmap.buffer, this->bitmap.size / 4096 + 1);

    for (int i = 0; i < mmap_entries; i++) {
        Memory_Map_Descriptor *desc = (Memory_Map_Descriptor *)((uint64_t)mmap + (i * mmap_desc_size));
        if (desc->type != 7) {
            pg_alloc_reserve_pages(this, (void *)desc->physical_start, desc->count);
        }
    }
}

void pg_alloc_init_bitmap(page_frame_allocator_t *this, size_t bitmap_size, void *buffer_address) {
    this->bitmap.size = bitmap_size;
    this->bitmap.buffer = (uint8_t *)buffer_address;

    for (int i = 0; i < bitmap_size; i++) {
        *(uint8_t *)(this->bitmap.buffer + i) = 0;
    }
}

void pg_alloc_free_page(page_frame_allocator_t *this, void *address) {
    uint64_t index = (uint64_t)address / 4096;

    if (bitmap_get(&this->bitmap, index) == false)
        return;

    bitmap_set(&this->bitmap, index, false);
    free_memory += 4096;
    used_memory -= 4096;
}
void pg_alloc_lock_page(page_frame_allocator_t *this, void *address) {
    uint64_t index = (uint64_t)address / 4096;

    if (bitmap_get(&this->bitmap, index) == true)
        return;

    bitmap_set(&this->bitmap, index, true);
    free_memory -= 4096;
    used_memory += 4096;
}
// private
void pg_alloc_unreserve_page(page_frame_allocator_t *this, void *address) {
    uint64_t index = (uint64_t)address / 4096;

    if (bitmap_get(&this->bitmap, index) == false)
        return;

    bitmap_set(&this->bitmap, index, false);
    free_memory += 4096;
    reserved_memory -= 4096;
}

void pg_alloc_reserve_page(page_frame_allocator_t *this, void *address) {
    uint64_t index = (uint64_t)address / 4096;

    if (bitmap_get(&this->bitmap, index) == true)
        return;

    bitmap_set(&this->bitmap, index, true);
    free_memory -= 4096;
    reserved_memory += 4096;
}

void pg_alloc_free_pages(page_frame_allocator_t *this, void *address, uint64_t page_count) {
    for (int t = 0; t < page_count; t++) {
        pg_alloc_free_page(this, (void *)((uint64_t)address + (t * 4096)));
    }
}
void pg_alloc_lock_pages(page_frame_allocator_t *this, void *address, uint64_t page_count) {
    for (int t = 0; t < page_count; t++) {
        pg_alloc_lock_page(this, (void *)((uint64_t)address + (t * 4096)));
    }
}
void pg_alloc_unreserve_pages(page_frame_allocator_t *this, void *address, uint64_t page_count) {
    for (int t = 0; t < page_count; t++) {
        pg_alloc_unreserve_page(this, (void *)((uint64_t)address + (t * 4096)));
    }
}
void pg_alloc_reserve_pages(page_frame_allocator_t *this, void *address, uint64_t page_count) {
    for (int t = 0; t < page_count; t++) {
        pg_alloc_reserve_page(this, (void *)((uint64_t)address + (t * 4096)));
    }
}

void *pg_alloc_request_page(page_frame_allocator_t *this) {
    for (uint64_t index = 0; index < this->bitmap.size * 8; index++) {
        if (bitmap_get(&this->bitmap, index) == true)
            continue;

        pg_alloc_lock_page(this, (void *)(index * 4096));
        return (void *)(index * 4096);
    }

    return NULL; // page frame, swap to file
}

uint64_t get_free_ram() { return free_memory; }
uint64_t get_used_ram() { return used_memory; }
uint64_t get_reserved_ram() { return reserved_memory; }