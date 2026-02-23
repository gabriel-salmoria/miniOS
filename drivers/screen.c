#include "drivers/screen.h"
#include "psf.h"

static framebuffer_info_t fb_internal;
static font_t font_internal;
static int cursor_x = 0;
static int cursor_y = 0;

void init_screen(boot_info_t *info) {
    fb_internal = *(info->fb);   /* Copy contents to safe kernel memory */
    font_internal = *(info->font);
    clear_screen();
}

void clear_screen() {
    uint32_t total_pixels = fb_internal.width * fb_internal.height;
    for (uint32_t i = 0; i < total_pixels; i++) {
        fb_internal.base_address[i] = 0x00000000;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || y < 0 || x >= (int)fb_internal.width || y >= (int)fb_internal.height) return;
    fb_internal.base_address[x + (y * fb_internal.pitch)] = color;
}

void print_char(char c, int x, int y, uint32_t color) {
    uint32_t height, width, charsize;
    uint8_t *glyph;

    /* Support both PSF1 and PSF2 formats */
    if (((uint8_t*)font_internal.psf_header)[0] == PSF1_MAGIC0) {
        psf1_header_t *h1 = (psf1_header_t*)font_internal.psf_header;
        height = h1->charsize;
        width = 8;
        charsize = h1->charsize;
        glyph = (uint8_t*)font_internal.glyph_buffer + (uint8_t)c * charsize;
    } else {
        psf2_header_t *h2 = (psf2_header_t*)font_internal.psf_header;
        height = h2->height;
        width = h2->width;
        charsize = h2->charsize;
        glyph = (uint8_t*)font_internal.glyph_buffer + (uint8_t)c * charsize;
    }

    uint32_t bytes_per_row = (width + 7) / 8;
    for (uint32_t cy = 0; cy < height; cy++) {
        for (uint32_t cx = 0; cx < width; cx++) {
            /* Bitmask: Check if the specific bit for this pixel is set */
            if (glyph[cy * bytes_per_row + (cx / 8)] & (0x80 >> (cx % 8))) {
                put_pixel(x + cx, y + cy, color);
            }
        }
    }
}

void kprint(char *message) {
    uint32_t height = 16; /* Default for lat0-16 */
    if (((uint8_t*)font_internal.psf_header)[0] == PSF2_MAGIC0) {
        height = ((psf2_header_t*)font_internal.psf_header)->height;
    }

    for (int i = 0; message[i] != 0; i++) {
        if (message[i] == '\n') {
            cursor_x = 0;
            cursor_y += height;
            continue;
        }
        print_char(message[i], cursor_x, cursor_y, 0xFFFFFFFF);
        cursor_x += 8;
        if (cursor_x + 8 > (int)fb_internal.width) {
            cursor_x = 0;
            cursor_y += height;
        }
    }
}

void kprint_backspace() {
    uint32_t height = 16; /* Default */
    if (((uint8_t*)font_internal.psf_header)[0] == PSF2_MAGIC0) {
        height = ((psf2_header_t*)font_internal.psf_header)->height;
    }

    if (cursor_x >= 8) {
        cursor_x -= 8;
        // Fill the 8xHeight area with black pixels
        for (uint32_t y = 0; y < height; y++) {
            for (int x = 0; x < 8; x++) {
                put_pixel(cursor_x + x, cursor_y + y, 0x00000000);
            }
        }
    }
}
