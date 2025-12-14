#ifndef HEAP_H
#define HEAP_H

#include "types.h"

typedef struct block_header {
    uint32_t size;
    uint8_t is_free;
    struct block_header *next;
} block_header_t;

void heap_init();
void *kmalloc(uint32_t size);
void kfree(void *ptr);

#endif
