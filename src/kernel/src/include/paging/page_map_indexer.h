#ifndef PAGE_MAP_INDEXER_H
#define PAGE_MAP_INDEXER_H

#include <stdint.h>

typedef struct __pagemap_indexer_t {
    uint64_t pdp_i;
    uint64_t pd_i;
    uint64_t pt_i;
    uint64_t p_i;
} pagemap_indexer_t;
typedef pagemap_indexer_t pm_indexer;

void pm_indexer_construct(pm_indexer *this, uint64_t virtual_addr);

#endif