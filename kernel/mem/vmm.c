#include "kernel/mem/vmm.h"
#include "kernel/mem/pmm.h"
#include "drivers/screen.h"
#include "kernel/cpu/isr.h"
#include "libc/string.h"

static page_table_t *pml4;

void load_page_directory(uint64_t directory_addr) {
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(directory_addr) : "memory");
}

void enable_paging() {
    uint64_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0) : "memory");
}

void page_fault_handler(registers_t *regs) {
    (void)regs;
    uint64_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r" (faulting_address));

    kprint("\n[PANIC] PAGE FAULT at 0x");
    char buf[32];
    hex_to_ascii(faulting_address, buf);
    kprint(buf);
    kprint("\nSystem Halted.\n");
    while(1) __asm__ __volatile__("hlt");
}

static page_table_t* get_next_table(page_table_t *current_table, uint64_t index) {
    if (current_table->entries[index].present) {
        return (page_table_t*)(current_table->entries[index].frame << 12);
    }

    page_table_t *new_table = (page_table_t*)pmm_alloc_page();
    memset((uint8_t*)new_table, 0, 4096);

    current_table->entries[index].frame = (uint64_t)new_table >> 12;
    current_table->entries[index].present = 1;
    current_table->entries[index].rw = 1;
    current_table->entries[index].user = 1;

    return new_table;
}

void vmm_map_page(uint64_t phys_addr, uint64_t virt_addr, uint64_t flags) {
    uint64_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (virt_addr >> 30) & 0x1FF;
    uint64_t pd_idx   = (virt_addr >> 21) & 0x1FF;
    uint64_t pt_idx   = (virt_addr >> 12) & 0x1FF;

    page_table_t *pdpt = get_next_table(pml4, pml4_idx);
    page_table_t *pd   = get_next_table(pdpt, pdpt_idx);
    page_table_t *pt   = get_next_table(pd, pd_idx);

    pt->entries[pt_idx].frame = phys_addr >> 12;
    pt->entries[pt_idx].present = (flags & 1) ? 1 : 0;
    pt->entries[pt_idx].rw      = (flags & 2) ? 1 : 0;
    pt->entries[pt_idx].user    = (flags & 4) ? 1 : 0;

    __asm__ __volatile__("invlpg (%0)" :: "r"(virt_addr) : "memory");
}

void init_vmm() {
    pml4 = (page_table_t*)pmm_alloc_page();
    memset((uint8_t*)pml4, 0, 4096);

    for (uint64_t i = 0; i < 512; i++) {
        vmm_map_page(i * 4096, i * 4096, 3);
    }

    register_interrupt_handler(14, page_fault_handler);
    load_page_directory((uint64_t)pml4);
    kprint("[VMM] - 4-Level Paging Initialized.\n");
}
