#include "kernel/mem/pmm.h"
#include "drivers/screen.h"

uint64_t *free_frames_stack;
uint64_t stack_top = 0;
uint64_t free_frames_count = 0;

void pmm_init(uint64_t mem_start, uint64_t mem_size) {
    if (mem_start % PAGE_SIZE != 0) {
        mem_start += PAGE_SIZE - (mem_start % PAGE_SIZE);
    }

    free_frames_stack = (uint64_t *)mem_start;

    uint64_t total_pages = mem_size / PAGE_SIZE;
    uint64_t stack_size_bytes = total_pages * sizeof(uint64_t);
    uint64_t first_free_page = mem_start + stack_size_bytes;

    if (first_free_page % PAGE_SIZE != 0) {
        first_free_page += PAGE_SIZE - (first_free_page % PAGE_SIZE);
    }

    stack_top = 0;
    for (int64_t i = total_pages - 1; i >= 0; i--) {
        uint64_t addr = first_free_page + (i * PAGE_SIZE);
        if (addr >= mem_start + mem_size) continue;

        free_frames_stack[stack_top++] = addr;
        free_frames_count++;
    }
    kprint("[PMM] - Physical Memory Initialized.\n");
}

void *pmm_alloc_page() {
    if (stack_top == 0) return 0;
    return (void *)free_frames_stack[--stack_top];
}

void pmm_free_page(void *page) {
    free_frames_stack[stack_top++] = (uint64_t)page;
}
