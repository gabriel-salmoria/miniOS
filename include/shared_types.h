#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <types.h>

typedef struct {
    uint32_t *base_address;
    uint64_t buffer_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
} framebuffer_info_t;

#endif
