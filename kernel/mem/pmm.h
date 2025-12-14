#ifndef PMM_H
#define PMM_H

#include <types.h>

#define PAGE_SIZE 4096

void pmm_init(uint32_t mem_start, uint32_t mem_size);
void *pmm_alloc_page();
void pmm_free_page(void *page);

#endif
