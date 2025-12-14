#include "drivers/pic.h"
#include "drivers/ports.h"

// Master/Slave PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void init_pic() {
    // ICW1: Start initialization
    port_byte_out(PIC1_COMMAND, 0x11);
    port_byte_out(PIC2_COMMAND, 0x11);

    // ICW2: Remap offsets
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

    // Mask interrupts (Unmask all)
    port_byte_out(PIC1_DATA, 0x0);
    port_byte_out(PIC2_DATA, 0x0);
}
