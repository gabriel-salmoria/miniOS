#ifndef PORTS_H
#define PORTS_H

#include <types.h>

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t data);

uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t data);

// Special helpers for ATA (Hard Drive)
void insw(uint16_t port, void *addr, int count);
void outsw(uint16_t port, void *addr, int count);

#endif
