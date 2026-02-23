#include "drivers/screen.h"

static framebuffer_info_t *screen_fb = 0;
static int cursor_x = 0;
static int cursor_y = 0;

void init_screen(framebuffer_info_t *fb) {
    screen_fb = fb;
}

void clear_screen() {
    if (!screen_fb) return;

    uint32_t total_pixels = screen_fb->width * screen_fb->height;
    for (uint32_t i = 0; i < total_pixels; i++) {
        screen_fb->base_address[i] = 0x00000000; // Black
    }
    cursor_x = 0;
    cursor_y = 0;
}

int print_char(char c, int col, int row, char attr) {
    // TODO: To print ASCII characters, we must load a bitmap font (e.g., PSF format).
    // For now, this draws a solid white 8x8 block to indicate text placement.
    if (!screen_fb) return 0;

    int px_start_x = col * 8;
    int px_start_y = row * 8;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            screen_fb->base_address[(px_start_x + x) + ((px_start_y + y) * screen_fb->pitch)] = 0x00FFFFFF;
        }
    }
    return 0; // Update offset math later
}

void kprint(char *message) {
    // Stub: Requires font rendering logic to increment cursor_x/y properly
}
