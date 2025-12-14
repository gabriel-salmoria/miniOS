#include "vmm.h"
#include "pmm.h"
#include "screen.h"

page_directory_t *kernel_directory;
page_table_t *first_page_table;

// Assembly helper to load CR3
void load_page_directory(uint32_t *directory_addr) {
    __asm__ __volatile__("mov %0, %%cr3" :: "r"(directory_addr));
}

// Assembly helper to set PG bit (bit 31) in CR0
void enable_paging() {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0" :: "r"(cr0));
}

void init_vmm() {
    // 1. Allocate a Page Directory
    kernel_directory = (page_directory_t *)pmm_alloc_page();

    // Clear it (ensure no garbage data)
    char *ptr = (char *)kernel_directory;
    for (int i = 0; i < 4096; i++) ptr[i] = 0;

    // 2. Allocate the first Page Table
    // This will cover Virtual 0x00000000 -> 0x00400000 (0-4MB)
    first_page_table = (page_table_t *)pmm_alloc_page();

    // 3. Identity Map the first 4MB
    // Loop 1024 times. Virtual 0 -> Physical 0, Virtual 4096 -> Physical 4096...
    for (int i = 0; i < 1024; i++) {
        first_page_table->entries[i].frame = i; // Map frame i to page i
        first_page_table->entries[i].present = 1;
        first_page_table->entries[i].rw = 1;   // Read/Write
        first_page_table->entries[i].user = 0; // Kernel Only
    }

    // 4. Link the Page Table to the Directory
    // Index 0 of the directory controls the first 4MB
    // We store the physical address of the table (shifted right by 12)
    kernel_directory->entries[0].table_addr = ((uint32_t)first_page_table) >> 12;
    kernel_directory->entries[0].present = 1;
    kernel_directory->entries[0].rw = 1;
    kernel_directory->entries[0].user = 0;

    // 5. Load CR3 and Enable Paging
    kprint("Enabling Paging...");
    load_page_directory((uint32_t *)kernel_directory);
    enable_paging();
    kprint("[OK]\n");
}
