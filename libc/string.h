#ifndef STRING_H
#define STRING_H

#include <types.h>

void mem_cpy(char *source, char *dest, int n_bytes);
void memset(uint8_t *dest, uint8_t val, uint32_t len);

int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
void append(char *s, char n);
void backspace(char *s);

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char *s);

#endif
