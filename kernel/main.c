#include "screen.h"
#include "keyboard.h"
#include "pic.h"
#include "isr.h"
#include "shell.h" // Import the shell

extern uint32_t end; // From linker (for PMM later)

void main() {
    clear_screen();
    kprint("ShitOS 32-bit Kernel Initializing...\n");

    // 1. Initialize Interrupts
    isr_install();

    // 2. Initialize Drivers
    init_keyboard();

    // 3. Handover to User Shell
    launch_shell();
}
