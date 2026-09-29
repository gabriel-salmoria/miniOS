#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <types.h>
#include "psf.h"

// Pixel format of the GOP framebuffer.
// Determines byte order when writing pixels.
typedef enum {
    FB_PIXEL_RGBX, // byte order: R G B unused
    FB_PIXEL_BGRX, // byte order: B G R unused (most common in QEMU + real hardware)
    FB_PIXEL_MASK, // custom masks, see pixel_mask_*
    FB_PIXEL_UNKNOWN,
} fb_pixel_format_t;

typedef struct {
    uint32_t *base_address;
    uint64_t  buffer_size;
    uint32_t  width;
    uint32_t  height;
    uint32_t  pitch;         // pixels per scan line (may differ from width)
    fb_pixel_format_t pixel_format;
    // only valid when pixel_format == FB_PIXEL_MASK
    uint32_t  pixel_mask_r;
    uint32_t  pixel_mask_g;
    uint32_t  pixel_mask_b;
} framebuffer_info_t;

// One entry in the memory map passed from bootloader to kernel.
// Use mmap_desc_size to step between entries, not sizeof(memory_map_entry_t).
typedef struct {
    uint32_t type;           // EFI_MEMORY_TYPE value
    uint32_t _pad;
    uint64_t phys_start;     // physical base address
    uint64_t virt_start;     // ignore for now
    uint64_t num_pages;      // size = num_pages * 4096
    uint64_t attributes;     // EFI_MEMORY_* flags
} memory_map_entry_t;

// Memory types the kernel PMM can treat as free RAM.
// EfiBootServicesCode/Data (3,4) are also free once ExitBootServices returns.
#define MMAP_FREE_TYPE(t) \
    ((t) == 1 || (t) == 2 || (t) == 3 || (t) == 4 || (t) == 5)

typedef struct {
    framebuffer_info_t *fb;
    font_t             *font;
    void               *rsdp;
    memory_map_entry_t *mmap;           // pointer to first descriptor
    uint64_t            mmap_size;      // total byte size of the map buffer
    uint64_t            mmap_desc_size; // bytes per entry (use this to iterate, not sizeof)
} boot_info_t;

#endif
