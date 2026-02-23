#ifndef SCREEN_H
#define SCREEN_H

#include "shared_types.h"

void init_screen(framebuffer_info_t *info);
void clear_screen();
void kprint(char *message);
void put_pixel(int x, int y, uint32_t color);

#endif
