#ifndef SCREEN_H
#define SCREEN_H

#include "shared_types.h"

#define MAX_ROWS 25 // Will be recalculated based on font and resolution
#define MAX_COLS 80

void init_screen(framebuffer_info_t *fb);
void clear_screen();
void kprint_at(char *message, int col, int row);
void kprint(char *message);

#endif
