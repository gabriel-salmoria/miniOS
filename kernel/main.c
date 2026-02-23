#include "drivers/screen.h"
#include "kernel/cpu/isr.h"
#include "drivers/keyboard.h"
#include "user/shell.h"
#include "kernel/mem/pmm.h"
#include "kernel/mem/vmm.h"
#include "kernel/mem/heap.h"
#include "kernel/sched/task.h"
#include "kernel/fs/ext2.h"

#include <shared_types.h>

extern uint32_t end;

void main(framebuffer_info_t *fb) {
    clear_screen();
    kprint("[MAIN] - ShitOS 32-bit Kernel Initializing...\n\n");

    isr_install();
    init_keyboard();

    // --- MEMORY INIT ---
    uint32_t kernel_end = (uint32_t)&end;
    uint32_t total_ram = 128 * 1024 * 1024;
    uint32_t pmm_start_addr = 0x10000;
    uint32_t free_mem = total_ram - pmm_start_addr;

    pmm_init(pmm_start_addr, free_mem);
    init_vmm();
    heap_init();


    // --- MULTITASKING INIT ---
    tasking_init(); // Main becomes PID 1


    ext2_init();


    // Create the Shell as PID 2
    create_task(launch_shell);


    // Enable Interrupts to start the Scheduler
    // (The Timer IRQ will now periodically force context switches)
    __asm__ __volatile__("sti");

    // --- IDLE LOOP ---
    // This is where main() goes to die. It just waits.
    // If the shell is blocked or waiting, the CPU runs this.
    while(1) {
        // 'hlt' puts the CPU in low-power mode until the next interrupt fires.
        __asm__ __volatile__("hlt");
    }
}
