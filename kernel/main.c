#include "screen.h"
#include "isr.h"
#include "keyboard.h"
#include "shell.h"
#include "mem/pmm.h"
#include "mem/vmm.h" // Import VMM
#include "string.h"

extern uint32_t end;

void main() {
    clear_screen();
    kprint("ShitOS 32-bit Kernel Initializing...\n");

    isr_install();
    init_keyboard();

    // --- MEMORY INIT ---
    uint32_t kernel_end = (uint32_t)&end;

    // FIX: Correctly calculate free memory size
    uint32_t total_ram = 128 * 1024 * 1024; // 128MB
    uint32_t pmm_start_addr = 0x10000;      // 64KB (Safe Zone)
    uint32_t free_mem = total_ram - pmm_start_addr;

    // DEBUG
    kprint("PMM Start: ");
    char buf[16];
    hex_to_ascii(pmm_start_addr, buf);
    kprint(buf);
    kprint("\n");

    // Initialize PMM with correct size
    pmm_init(pmm_start_addr, free_mem);

    // Initialize VMM (Paging)
    init_vmm();
    // -------------------

    launch_shell();
}
