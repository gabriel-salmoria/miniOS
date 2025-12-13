#include "../drivers/screen.h"
#include "../include/isr.h" // <--- Include this

void main() {
    clear_screen();
    kprint_at("ShitOS 32-bit Kernel", 0, 0);
    kprint("\n\nInitializing drivers...\n");

    // Initialize Interrupts (IDT + PIC + Enable STI)
    isr_install();

    kprint("IDT:           [OK]\n");
    kprint("Keyboard:      [ACTIVE] (Try typing!)\n");
}
