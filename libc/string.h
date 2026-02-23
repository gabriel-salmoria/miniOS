#ifndef STRING_H
#define STRING_H

#include <types.h>

void *memcpy(void *source, const void *dest, int n_bytes);
void memset(void *dest, uint8_t val, uint64_t len);

int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
void append(char *s, char n);
void backspace(char *s);

void int_to_ascii(int64_t n, char str[]);
void hex_to_ascii(uint64_t n, char str[]);
void reverse(char *s);

#endif
