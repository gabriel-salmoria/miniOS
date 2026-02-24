#include "drivers/apic.h"
#include "kernel/acpi/acpi.h"
#include "drivers/ports.h"

#define LAPIC_ID         0x0020
#define LAPIC_EOI        0x00B0
#define LAPIC_SIVR       0x00F0

static void *apic_base = 0;

void apic_write(uint32_t reg, uint32_t val) {
    *((volatile uint32_t *)((uint64_t)apic_base + reg)) = val;
}

uint32_t apic_read(uint32_t reg) {
    return *((volatile uint32_t *)((uint64_t)apic_base + reg));
}

void apic_eoi() {
    apic_write(LAPIC_EOI, 0);
}

void disable_pic() {
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

void init_apic() {
    apic_base = acpi_get_lapic_base();
    if (!apic_base) return;

    disable_pic();
    apic_write(LAPIC_SIVR, apic_read(LAPIC_SIVR) | 0x100 | 0xFF);
}
