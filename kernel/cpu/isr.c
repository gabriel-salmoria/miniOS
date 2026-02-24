#include "kernel/cpu/isr.h"
#include "drivers/apic/apic.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(registers_t *r) {
    // Send End of Interrupt to Local APIC
    if (r->int_no >= 32) {
        apic_eoi();
    }

    if (interrupt_handlers[r->int_no] != 0) {
        isr_t handler = interrupt_handlers[r->int_no];
        handler(r);
    }
}
