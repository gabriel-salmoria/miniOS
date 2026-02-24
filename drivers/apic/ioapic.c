#include "kernel/acpi/acpi.h"
#include <types.h>

static volatile uint32_t *ioapic_mmio = 0;

void ioapic_write(uint8_t reg, uint32_t val) {
    ioapic_mmio[0] = reg;
    ioapic_mmio[4] = val;
}

void ioapic_set_entry(uint8_t index, uint64_t data) {
    uint8_t reg = 0x10 + (index * 2);
    ioapic_write(reg, (uint32_t)data);
    ioapic_write(reg + 1, (uint32_t)(data >> 32));
}

void init_ioapic() {
    ioapic_mmio = (volatile uint32_t *)acpi_get_ioapic_base();
    if (!ioapic_mmio) return;

    // Route IRQ 1 (Keyboard) to Vector 33
    // Format: Vector(33) | Delivery Mode(0) | Unmasked(0)
    uint64_t kb_entry = 33;
    ioapic_set_entry(1, kb_entry);
}
