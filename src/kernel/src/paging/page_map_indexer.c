#include <paging/page_map_indexer.h>

void pm_indexer_construct(pm_indexer *this, uint64_t virtual_addr) {
    virtual_addr >>= 12;
    this->p_i = virtual_addr & 0x1ff;
    virtual_addr >>= 9;
    this->pt_i = virtual_addr & 0x1ff;
    virtual_addr >>= 9;
    this->pd_i = virtual_addr & 0x1ff;
    virtual_addr >>= 9;
    this->pdp_i = virtual_addr & 0x1ff;
}