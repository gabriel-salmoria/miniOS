#include "kernel/cpu/isr.h"
#include "drivers/ports.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(registers_t *r) {
    // 1. Send EOI to PIC IMMEDIATELY for hardware interrupts (IRQ 0-15)
    // This allows the hardware to prep for the next interrupt even if we switch tasks
    if (r->int_no >= 32 && r->int_no <= 47) {
        if (r->int_no >= 40) outb(0xA0, 0x20); // Slave
        outb(0x20, 0x20); // Master
    }

    // 2. Execute the handler
    if (interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}
