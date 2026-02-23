#ifndef SCREEN_H
#define SCREEN_H

#include "shared_types.h"

#include <psf.h>


void init_screen(boot_info_t *info);
void clear_screen();
void kprint(char *message);

#endif
