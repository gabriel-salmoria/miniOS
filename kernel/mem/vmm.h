#ifndef VMM_H
#define VMM_H

#include <types.h>
#include "shared_types.h"

typedef struct {
    uint64_t present    : 1;
    uint64_t rw         : 1;
    uint64_t user       : 1;
    uint64_t write_thru : 1;
    uint64_t cache_dis  : 1;
    uint64_t accessed   : 1;
    uint64_t dirty      : 1;
    uint64_t huge_page  : 1;
    uint64_t global     : 1;
    uint64_t avail      : 3;
    uint64_t frame      : 40; // Physical address bits 12-51
    uint64_t reserved   : 11;
    uint64_t nx         : 1;  // No-Execute bit
} __attribute__((packed)) page_entry_t;

typedef struct {
    page_entry_t entries[512];
} __attribute__((aligned(4096))) page_table_t;

void init_vmm();
void vmm_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags);

#endif
