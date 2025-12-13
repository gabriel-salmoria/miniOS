#include "../include/isr.h"
#include "../drivers/screen.h"
#include "../drivers/ports.h"
#include "../drivers/keyboard.h" // <--- Ensure this is included

// ... keep int_to_ascii ...
void int_to_ascii(int n, char str[]) {
    int i = 0;
    if (n == 0) { str[0] = '0'; str[1] = '\0'; return; }

    char temp[16];
    int j = 0;
    while (n > 0) {
        temp[j++] = (n % 10) + '0';
        n /= 10;
    }
    while (j > 0) str[i++] = temp[--j];
    str[i] = '\0';
}

// Defined in drivers/keyboard.c
extern void handle_keyboard_interrupt();

void isr_handler(registers_t r) {
    // Exception Handlers (0-31)
    if (r.int_no < 32) {
        char s[16];
        kprint("EXCEPTION: ");
        int_to_ascii(r.int_no, s);
        kprint(s);
        kprint("\n");
    }
    // IRQ 0: Timer (32)
    else if (r.int_no == 32) {
        // Do nothing (stop printing 'T')
    }
    // IRQ 1: Keyboard (33)
    else if (r.int_no == 33) {
        handle_keyboard_interrupt(); // <--- CRITICAL: Pass control to driver
    }

    // Send End of Interrupt (EOI) to PICs
    // If interrupt came from Slave PIC (>= 40), send EOI to Slave
    if (r.int_no >= 40) port_byte_out(0xA0, 0x20);
    // Always send EOI to Master PIC
    port_byte_out(0x20, 0x20);
}
