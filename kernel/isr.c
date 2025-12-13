#include "../include/isr.h"
#include "../drivers/screen.h"
#include "../drivers/ports.h"

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

void isr_handler(registers_t r) {
    // Exception 0-31
    if (r.int_no < 32) {
        char s[16];
        kprint("EXCEPTION: ");
        int_to_ascii(r.int_no, s);
        kprint(s);
        kprint("\n");
    }
    // IRQ 0 (Timer)
    else if (r.int_no == 32) {
        kprint("T"); // Commented out to keep screen clean
    }
    // IRQ 1 (Keyboard)
    else if (r.int_no == 33) {
        uint8_t scancode = port_byte_in(0x60); // <--- CRITICAL: Read data to clear buffer

        kprint("Key: ");
        char s[8];
        int_to_ascii(scancode, s);
        kprint(s);
        kprint(" ");
    }

    // Send EOI to PICs
    if (r.int_no >= 40) port_byte_out(0xA0, 0x20); // Slave
    port_byte_out(0x20, 0x20); // Master
}
