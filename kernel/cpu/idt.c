#include "kernel/cpu/idt.h"
#include "drivers/pic.h"

idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

void set_idt_gate(int n, uint64_t handler) {
    idt[n].offset_low = (uint16_t)(handler & 0xFFFF);
    idt[n].selector = KERNEL_CS; // 0x08
    idt[n].ist = 0;
    idt[n].type_attr = 0x8E; // Interrupt Gate, Privilege 0
    idt[n].offset_mid = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[n].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    idt[n].reserved = 0;
}

void set_idt() {
    idt_reg.base = (uint64_t)&idt;
    idt_reg.limit = (sizeof(idt_gate_t) * IDT_ENTRIES) - 1;
    __asm__ __volatile__("lidt %0" : : "m"(idt_reg));
}

extern uint64_t isr_stub_table[]; // Use uint64_t for the table

void isr_install() {
    set_idt();

    for (int i = 0; i < 48; i++) {
        set_idt_gate(i, isr_stub_table[i]); // Pass the 64-bit address
    }

    init_pic();
}
