#include "kernel/mem/heap.h"
#include "kernel/mem/pmm.h"
#include "kernel/mem/vmm.h"
#include "drivers/screen.h"

// Move heap to 4GB to avoid the 4GB Huge Page identity map conflict
#define KHEAP_START         0x100000000ULL
#define KHEAP_INITIAL_SIZE  0x100000
#define MIN_BLOCK_SIZE      16

block_header_t *start_tag = 0;

void heap_init() {
    uint64_t current_addr = KHEAP_START;
    uint64_t end_addr = KHEAP_START + KHEAP_INITIAL_SIZE;

    while (current_addr < end_addr) {
        void *phys_frame = pmm_alloc_page();
        vmm_map_page((uint64_t)phys_frame, current_addr, 3);
        current_addr += 4096;
    }

    start_tag = (block_header_t *)KHEAP_START;
    start_tag->size = KHEAP_INITIAL_SIZE - sizeof(block_header_t);
    start_tag->is_free = 1;
    start_tag->next = 0;

    kprint("[HEAP] - Heap Initialized.\n");
}

void *kmalloc(uint32_t size) {
    if (size == 0) return 0;
    if (size % 8 != 0) size += 8 - (size % 8);

    block_header_t *current = start_tag;
    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size > size + sizeof(block_header_t) + MIN_BLOCK_SIZE) {
                block_header_t *new_block = (block_header_t *)((uint64_t)current + sizeof(block_header_t) + size);
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->is_free = 1;
                new_block->next = current->next;
                current->size = size;
                current->next = new_block;
            }
            current->is_free = 0;
            return (void *)((uint64_t)current + sizeof(block_header_t));
        }
        current = current->next;
    }
    return 0;
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_header_t *header = (block_header_t *)((uint64_t)ptr - sizeof(block_header_t));
    header->is_free = 1;

    block_header_t *current = start_tag;
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += current->next->size + sizeof(block_header_t);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
