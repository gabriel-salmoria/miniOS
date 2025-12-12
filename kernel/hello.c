#include "../drivers/screen.h"

void main() {
    clear_screen();
    kprint_at("ShitOS 32-bit Kernel", 0, 0);
    kprint("\n\nInitializing drivers...\n");
    kprint("Screen driver: OK\n");
    kprint("Port I/O:      OK\n");
    kprint("Keyboard:      PENDING (Requires IDT)");
}
