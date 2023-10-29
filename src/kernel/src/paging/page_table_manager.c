#include <paging/page_table_manager.h>
#include <paging/page_map_indexer.h>
#include <paging/page_frame_allocator.h>
#include <stdint.h>

void pt_manager_construct(pt_manager *this, page_table_t *pml4_addr) {
    this->pml4_addr = pml4_addr;
}

void pt_manager_map_memory(pt_manager *this, void *virtual_memory, void *physical_memory) {
    pm_indexer indexer;
    pm_indexer_construct(&indexer, (uint64_t)virtual_memory);
    page_dir_entry_t pde;

    pde = this->pml4_addr->entries[indexer.pdp_i];
    page_table_t *pdp;
    if (!pde.present) {
        pdp = (page_table_t *)pg_alloc_request_page(&global_allocator);
        m_memset(pdp, 0, 0x1000);
        pde.address = (uint64_t)pdp >> 12;
        pde.present = true;
        pde.read_write = true;
        this->pml4_addr->entries[indexer.pdp_i] = pde;
    } else {
        pdp = (page_table_t *)((uint64_t)pde.address << 12);
    }

    //
    pde = pdp->entries[indexer.pd_i];
    page_table_t *pd;
    if (!pde.present) {
        pd = (page_table_t *)pg_alloc_request_page(&global_allocator);
        m_memset(pd, 0, 0x1000);
        pde.address = (uint64_t)pd >> 12;
        pde.present = true;
        pde.read_write = true;
        pdp->entries[indexer.pd_i] = pde;
    } else {
        pd = (page_table_t *)((uint64_t)pde.address << 12);
    }

    pde = pd->entries[indexer.pt_i];
    page_table_t *pt;
    if (!pde.present) {
        pt = (page_table_t *)pg_alloc_request_page(&global_allocator);
        m_memset(pt, 0, 0x1000);
        pde.address = (uint64_t)pt >> 12;
        pde.present = true;
        pde.read_write = true;
        pd->entries[indexer.pt_i] = pde;
    } else {
        pt = (page_table_t *)((uint64_t)pde.address << 12);
    }

    pde = pt->entries[indexer.p_i];
    pde.address = (uint64_t)physical_memory >> 12;
    pde.present = true;
    pde.read_write = true;
    pt->entries[indexer.p_i] = pde;
}
