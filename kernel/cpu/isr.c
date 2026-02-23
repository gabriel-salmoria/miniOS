#include "kernel/cpu/isr.h"
#include "drivers/ports.h"
#include "kernel/cpu/isr.h"
#include "drivers/ports.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}


// Update signature to use a pointer
void isr_handler(registers_t *r) {
    if (interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }

    // Ack PICs (IRQ 0-15)
    if (r->int_no >= 32 && r->int_no <= 47) {
        if (r->int_no >= 40) outb(0xA0, 0x20);
        outb(0x20, 0x20);
    }
}
