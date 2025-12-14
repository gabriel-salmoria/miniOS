#ifndef VMM_H
#define VMM_H

#include "types.h"

// 1. Page Table Entry (PTE) - 32 bits total
typedef struct {
    uint32_t present    : 1;  // Page present in memory
    uint32_t rw         : 1;  // Read-only if clear, read-write if set
    uint32_t user       : 1;  // Supervisor level only if clear
    uint32_t write_thru : 1;
    uint32_t cache_dis  : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t pat        : 1;
    uint32_t global     : 1;
    uint32_t avail      : 3;  // Available for kernel use
    uint32_t frame      : 20; // Physical Frame Address (High 20 bits)
} __attribute__((packed)) pte_t;

// 2. Page Directory Entry (PDE) - 32 bits total
typedef struct {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t write_thru : 1;
    uint32_t cache_dis  : 1;
    uint32_t accessed   : 1;
    uint32_t reserved   : 1;  // 0
    uint32_t page_size  : 1;  // 0 for 4KB
    uint32_t global     : 1;  // Ignored
    uint32_t avail      : 3;
    uint32_t table_addr : 20; // Physical address of Page Table
} __attribute__((packed)) pde_t;

// 3. The Page Directory (1024 entries * 4 bytes = 4KB)
typedef struct {
    pde_t entries[1024];
} __attribute__((aligned(4096))) page_directory_t;

// 4. A Page Table (1024 entries * 4 bytes = 4KB)
typedef struct {
    pte_t entries[1024];
} __attribute__((aligned(4096))) page_table_t;

void init_vmm();
void vmm_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags);


#endif
