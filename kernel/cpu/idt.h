#ifndef IDT_H
#define IDT_H

#include <types.h>

#define KERNEL_CS 0x08
#define IDT_ENTRIES 256

typedef struct {
    uint16_t offset_low;    // Offset bits 0..15
    uint16_t selector;      // A code segment selector in GDT
    uint8_t  ist;           // Interrupt Stack Table offset
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_mid;    // Offset bits 16..31
    uint32_t offset_high;   // Offset bits 32..63
    uint32_t reserved;      // Reserved
} __attribute__((packed)) idt_gate_t;

typedef struct {
    uint16_t limit;
    uint64_t base;          // Expanded to 64-bit
} __attribute__((packed)) idt_register_t;

void set_idt_gate(int n, uint64_t handler);
void set_idt();

#endif
