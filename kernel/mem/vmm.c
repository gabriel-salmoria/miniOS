#include "vmm.h"
#include "pmm.h"
#include "screen.h"
#include "isr.h"    // Needed for registering the handler
#include "string.h" // For hex_to_ascii

page_directory_t *kernel_directory;
page_table_t *first_page_table;

// --- Helpers ---
void load_page_directory(uint32_t *directory_addr) {
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(directory_addr));
}

void enable_paging() {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
}

void page_fault_handler(registers_t *regs) {
    uint32_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r" (faulting_address));

    // Error code interpretation
    // Bit 0: 0 = Not Present, 1 = Protection Violation
    int not_present = !(regs->err_code & 0x1);
    int rw = regs->err_code & 0x2;
    int us = regs->err_code & 0x4;
    int reserved = regs->err_code & 0x8;

    kprint("\n[PANIC] PAGE FAULT! ( ");
    if (not_present) kprint("not-present "); // Clearer message
    if (rw) kprint("read-only ");
    if (us) kprint("user-mode ");
    if (reserved) kprint("reserved ");
    kprint(") at 0x");

    char buf[16];
    buf[0] = '\0'; // FIX: Initialize buffer to empty string
    hex_to_ascii(faulting_address, buf);
    kprint(buf);
    kprint("\n");

    kprint("System Halted.\n");
    __asm__ __volatile__("hlt");
}

void init_vmm() {
    // 1. Allocate Directory
    kernel_directory = (page_directory_t *)pmm_alloc_page();

    // Clear directory
    char *ptr = (char *)kernel_directory;
    for (int i = 0; i < 4096; i++) ptr[i] = 0;

    // 2. Allocate First Page Table
    first_page_table = (page_table_t *)pmm_alloc_page();

    // 3. Identity Map 0-4MB
    for (int i = 0; i < 1024; i++) {
        first_page_table->entries[i].frame = i;
        first_page_table->entries[i].present = 1;
        first_page_table->entries[i].rw = 1;
        first_page_table->entries[i].user = 0;
    }

    // 4. Link Table to Directory
    kernel_directory->entries[0].table_addr = ((uint32_t)first_page_table) >> 12;
    kernel_directory->entries[0].present = 1;
    kernel_directory->entries[0].rw = 1;
    kernel_directory->entries[0].user = 0;

    // 5. Register Handler
    register_interrupt_handler(14, page_fault_handler);

    // 6. Enable Paging
    kprint("Enabling Paging...");
    load_page_directory((uint32_t *)kernel_directory);
    enable_paging();
    kprint("[OK]\n");

}


void vmm_map_page(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags) {
    // 1. Calculate Indices
    uint32_t pd_index = virt_addr >> 22;
    uint32_t pt_index = (virt_addr >> 12) & 0x03FF;

    // 2. Check if the Page Table exists
    // We check the 'present' bit of the directory entry
    if (kernel_directory->entries[pd_index].present == 0) {
        // NO TABLE: We must allocate one
        // CRITICAL: This requires the PMM to return a Low Memory address (< 4MB)
        // or this pointer will point to unmapped memory and crash.
        page_table_t *new_table = (page_table_t *)pmm_alloc_page();

        if (!new_table) return; // OOM Safety check

        // Clear it manually to prevent garbage data causing random crashes
        char *ptr = (char *)new_table;
        for (int i = 0; i < 4096; i++) ptr[i] = 0;

        // Map the new table into the directory
        // The table_addr is physical (which pmm_alloc_page returns)
        kernel_directory->entries[pd_index].table_addr = ((uint32_t)new_table) >> 12;
        kernel_directory->entries[pd_index].present = 1;
        kernel_directory->entries[pd_index].rw = 1;
        kernel_directory->entries[pd_index].user = 1; // Allow user access to table container
    }

    // 3. Get the table
    // Since we are identity mapped, the physical address *is* the pointer
    uint32_t table_phys = kernel_directory->entries[pd_index].table_addr << 12;
    page_table_t *table = (page_table_t *)table_phys;

    // 4. Map the Page
    table->entries[pt_index].frame = phys_addr >> 12;
    table->entries[pt_index].present = (flags & 1) ? 1 : 0;
    table->entries[pt_index].rw      = (flags & 2) ? 1 : 0;
    table->entries[pt_index].user    = (flags & 4) ? 1 : 0;

    // 5. Flush TLB
    // We reload CR3 to force the CPU to see the new mapping
    load_page_directory((uint32_t *)kernel_directory);
}
