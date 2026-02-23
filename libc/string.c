#include "libc/string.h"
#include <types.h>

// --- Memory Functions ---


void *memcpy(void *dest, const void *src, int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    for (int i = 0; i < n; i++)
        d[i] = s[i];

    return dest;
}


void memset(void *dest, uint8_t val, uint64_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

// --- String Functions ---

int strlen(const char *s) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

int strcmp(const char *s1, const char *s2) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

void append(char *s, char n) {
    int len = strlen(s);
    s[len] = n;
    s[len + 1] = '\0';
}

void backspace(char *s) {
    int len = strlen(s);
    if (len > 0) s[len - 1] = '\0';
}

// --- Conversion Functions ---

void reverse(char *s) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void int_to_ascii(int64_t n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(uint64_t n, char str[]) {
    str[0] = '\0'; // Start with empty string
    append(str, '0');
    append(str, 'x');
    int zeros = 0;

    // Process 64 bits (16 hex chars)
    for (int i = 60; i >= 0; i -= 4) {
        uint8_t tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0 && i > 0) continue;
        zeros = 1;
        if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }
}
