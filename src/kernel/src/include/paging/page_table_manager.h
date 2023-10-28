#ifndef PAGE_TABLE_MANAGER_H
#define PAGE_TABLE_MANAGER_H

#include "paging.h"

typedef struct __page_table_manager_t {
    page_table_t *pml4_addr;
} page_table_manager_t;

typedef page_table_manager_t pt_manager;

void pt_manager_construct(pt_manager *this, page_table_t *pml4_addr);
void pt_manager_map_memory(pt_manager *this, void *virtual_memory, void *physical_memory);

#endif