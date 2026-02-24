#include "drivers/apic/ports.h"
#include <types.h>

/**
 * Read a byte from the specified port
 */
uint8_t inb(uint16_t port) {
    uint8_t result;
    // "=a" (result) means: put the AL register value into 'result'
    // "d" (port) means: load 'port' into EDX register
    __asm__("inb %1, %0" : "=a" (result) : "d" (port));
    return result;
}

/**
 * Write a byte to the specified port
 */
void outb(uint16_t port, uint8_t data) {
    // "a" (data) means: load 'data' into AL register
    // "d" (port) means: load 'port' into EDX register
    __asm__("outb %0, %1" : : "a" (data), "d" (port));
}

/**
 * Read a word (2 bytes) from the specified port
 */
uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__("inw %1, %0" : "=a" (result) : "d" (port));
    return result;
}

/**
 * Write a word (2 bytes) to the specified port
 */
void outw(uint16_t port, uint16_t data) {
    __asm__("outw %0, %1" : : "a" (data), "d" (port));
}

/**
 * Read 'count' words from a port into a buffer (CRITICAL for ATA Driver)
 * This is much faster than a C loop calling inw() repeatedly.
 */
void insw(uint16_t port, void *addr, int count) {
    __asm__ volatile ("cld; rep insw" : "+D" (addr), "+c" (count) : "d" (port) : "memory");
}

/**
 * Write 'count' words from a buffer to a port
 */
void outsw(uint16_t port, void *addr, int count) {
    __asm__ volatile ("cld; rep outsw" : "+S" (addr), "+c" (count) : "d" (port));
}
