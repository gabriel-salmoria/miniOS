#include "idt.h"       // Changed from "../include/idt.h"
#include "isr.h"       // Changed from "../include/isr.h"
#include "pic.h"       // New include (compiler finds it in 'drivers/')

// 1. Define the IDT globally
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

// 2. Import the array of pointers from assembly
extern void (*isr_stub_table[])(void);

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = low_16(handler);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].high_offset = high_16(handler);
}

void set_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__("lidt (%0)" : : "r" (&idt_reg));
}

void isr_install() {
    set_idt();

    // Install handlers 0-47
    for (int i = 0; i < 48; i++) {
        set_idt_gate(i, (uint32_t)isr_stub_table[i]);
    }

    init_pic(); // Now calls the driver
    __asm__ __volatile__("sti");
}
