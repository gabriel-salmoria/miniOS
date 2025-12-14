#include "drivers/pic.h"
#include "drivers/ports.h"

// Master/Slave PIC ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void init_pic() {
    // ICW1: Start initialization
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // ICW2: Remap offsets
    // Master PIC (IRQ 0-7) starts at 32 (0x20)
    outb(PIC1_DATA, 0x20);
    // Slave PIC (IRQ 8-15) starts at 40 (0x28)
    outb(PIC2_DATA, 0x28);

    // ICW3: Cascade setup
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    // ICW4: Environment (8086 mode)
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Mask interrupts (Unmask all)
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}
