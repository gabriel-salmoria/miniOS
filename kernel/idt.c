#include "../include/idt.h"
#include "../include/isr.h"
#include "../drivers/ports.h" // We need port I/O now

// --- PIC CONSTANTS ---
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// 1. Define the IDT globally
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;

// 2. Import the array of pointers from assembly
extern void (*isr_stub_table[32])(void);

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

// --- NEW FUNCTION: Remap PIC ---
void init_pic() {
    // ICW1: Start initialization
    port_byte_out(PIC1_COMMAND, 0x11);
    port_byte_out(PIC2_COMMAND, 0x11);

    // ICW2: Remap offsets (The important part!)
    // Master PIC (IRQ 0-7) starts at 32 (0x20)
    port_byte_out(PIC1_DATA, 0x20);
    // Slave PIC (IRQ 8-15) starts at 40 (0x28)
    port_byte_out(PIC2_DATA, 0x28);

    // ICW3: Cascade setup
    port_byte_out(PIC1_DATA, 0x04);
    port_byte_out(PIC2_DATA, 0x02);

    // ICW4: Environment (8086 mode)
    port_byte_out(PIC1_DATA, 0x01);
    port_byte_out(PIC2_DATA, 0x01);

    // Mask interrupts (Optional: Unmask only what we need later)
    // For now, let's unmask everything (0x0 = all enabled)
    port_byte_out(PIC1_DATA, 0x0);
    port_byte_out(PIC2_DATA, 0x0);
}

void isr_install() {
    set_idt();

    // Install the first 32 CPU exception handlers + 16 IRQs
    // CHANGE THIS LIMIT FROM 32 TO 48
    for (int i = 0; i < 48; i++) {
        set_idt_gate(i, (uint32_t)isr_stub_table[i]);
    }

    init_pic();
    __asm__ __volatile__("sti");
}
