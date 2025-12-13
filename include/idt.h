#ifndef IDT_H
#define IDT_H

#include "types.h"

#define KERNEL_CS 0x08
#define IDT_ENTRIES 256

// 1. The Struct: One entry in the IDT
typedef struct {
    uint16_t low_offset;   // Lower 16 bits of handler function address
    uint16_t sel;          // Kernel segment selector
    uint8_t  always0;

    /* First byte
     * Bit 7: "Present"
     * Bits 6-5: Privilege (0=kernel..3=user)
     * Bit 4: Set to 0 for interrupt gates
     * Bits 3-0: bits 1110 = decimal 14 = "32 bit interrupt gate" */
    uint8_t  flags;
    uint16_t high_offset;  // Higher 16 bits of handler function address
} __attribute__((packed)) idt_gate_t;

// 2. The Pointer: What we pass to the 'lidt' assembly instruction
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_register_t;

// Functions implemented in kernel/idt.c
void set_idt_gate(int n, uint32_t handler);
void set_idt();

#endif
