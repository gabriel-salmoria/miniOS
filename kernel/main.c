#include "screen.h"
#include "isr.h"
#include "keyboard.h"
#include "shell.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "mem/heap.h" // Import Heap

extern uint32_t end;

void main() {
    clear_screen();
    kprint("ShitOS 32-bit Kernel Initializing...\n");

    isr_install();
    init_keyboard();

    // --- MEMORY INIT ---
    uint32_t kernel_end = (uint32_t)&end;
    uint32_t total_ram = 128 * 1024 * 1024;
    uint32_t pmm_start_addr = 0x10000;
    uint32_t free_mem = total_ram - pmm_start_addr;

    // 1. PMM
    pmm_init(pmm_start_addr, free_mem);

    // 2. VMM
    init_vmm();

    // 3. HEAP (New)
    heap_init();

    launch_shell();
}
