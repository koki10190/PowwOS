#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct __page_dir_entry_t {
    bool present : 1;
    bool read_write : 1;
    bool user_super : 1;
    bool write_through : 1;
    bool cache_disabled : 1;
    bool accessed : 1;
    bool ignore0 : 1;
    bool larger_pages : 1;
    bool ignore1 : 1;
    uint8_t available : 3;
    uint64_t address : 52;
} page_dir_entry_t;

typedef struct __page_table_t {
    page_dir_entry_t entries[512];
} __attribute__((aligned(0x1000))) page_table_t;

#endif