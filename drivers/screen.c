#include "drivers/screen.h"

static framebuffer_info_t fb_internal;
static framebuffer_info_t *fb = &fb_internal;
static int cursor_x = 0;
static int cursor_y = 0;

void init_screen(framebuffer_info_t *info) {
    fb_internal = *info;
    clear_screen();
}

void clear_screen() {
    if (!fb) return;
    for (uint32_t i = 0; i < fb->width * fb->height; i++) {
        fb->base_address[i] = 0x00000000;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void put_pixel(int x, int y, uint32_t color) {
    fb->base_address[x + (y * fb->pitch)] = color;
}

void print_char(char c, int x, int y, uint32_t color) {
    // Basic 8x8 bitmask rendering (Stub: currently draws a solid square)
    // Replace with PSF font parsing logic later
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void kprint(char *message) {
    if (!fb) return;

    for (int i = 0; message[i] != 0; i++) {
        if (message[i] == '\n') {
            cursor_x = 0;
            cursor_y += 12; // Character height + padding
            continue;
        }

        print_char(message[i], cursor_x, cursor_y, 0xFFFFFFFF);
        cursor_x += 8; // Character width

        if (cursor_x + 8 > (int)fb->width) {
            cursor_x = 0;
            cursor_y += 12;
        }
    }
}
