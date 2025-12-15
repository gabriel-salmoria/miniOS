#include "kernel/mem/pmm.h"
#include "drivers/screen.h" // For kprint

uint32_t *free_frames_stack;
uint32_t stack_top = 0;
uint32_t free_frames_count = 0;

void pmm_init(uint32_t mem_start, uint32_t mem_size) {
    // 1. Align start address to 4KB
    if (mem_start % PAGE_SIZE != 0) {
        mem_start += PAGE_SIZE - (mem_start % PAGE_SIZE);
    }

    // 2. Place the stack at the start of free memory
    free_frames_stack = (uint32_t *)mem_start;

    // 3. Calculate space required for the stack itself
    uint32_t total_pages = mem_size / PAGE_SIZE;
    uint32_t stack_size_bytes = total_pages * sizeof(uint32_t);

    // 4. The actual free pages start AFTER our stack
    uint32_t first_free_page = mem_start + stack_size_bytes;

    // Re-align
    if (first_free_page % PAGE_SIZE != 0) {
        first_free_page += PAGE_SIZE - (first_free_page % PAGE_SIZE);
    }

    // 5. Fill the stack (REVERSE ORDER FIX)
    // We iterate backwards. By pushing High Addresses first, they end up at the
    // bottom of the stack. Low Addresses end up at the top.
    // So pmm_alloc_page() will return Low Addresses (safe) first.
    stack_top = 0;
    for (int32_t i = total_pages - 1; i >= 0; i--) {
        uint32_t addr = first_free_page + (i * PAGE_SIZE);

        // Safety check: Don't exceed physical RAM
        if (addr >= mem_start + mem_size) continue;

        free_frames_stack[stack_top] = addr;
        stack_top++;
        free_frames_count++;
    }

    kprint("[PMM] - Physical Memory Initialized. \n");
}

void *pmm_alloc_page() {
    if (stack_top == 0) {
        kprint("PMM: Out of Memory!\n");
        return 0;
    }

    stack_top--;
    free_frames_count--;
    return (void *)free_frames_stack[stack_top];
}

void pmm_free_page(void *page) {
    uint32_t addr = (uint32_t)page;
    free_frames_stack[stack_top] = addr;
    stack_top++;
    free_frames_count++;
}
