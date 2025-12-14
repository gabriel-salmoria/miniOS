#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <types.h>

void print_letter(uint8_t scancode);
void init_keyboard();
int is_input_complete();
void get_keyboard_input(char *target_buffer);

#endif
