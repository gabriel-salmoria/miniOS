#include "drivers/screen.h"
#include "kernel/cpu/isr.h"
#include "drivers/keyboard.h"
#include "user/shell.h"
#include "kernel/mem/pmm.h"
#include "kernel/mem/vmm.h"
#include "kernel/mem/heap.h"
#include "kernel/sched/task.h"
#include "kernel/cpu/gdt.h"
#include "libc/string.h" // Fix: use local header
#include <shared_types.h>

extern uint64_t _bss_start;
extern uint64_t _bss_end;

__attribute__((ms_abi))
void main(boot_info_t *boot_info) {
    // 1. Clear BSS immediately
    uint64_t bss_size = (uint64_t)&_bss_end - (uint64_t)&_bss_start;
    memset(&_bss_start, 0, bss_size);

    init_screen(boot_info);
    kprint("ShitOS 64-bit: Kernel Started\n"); // Debug print

    init_gdt();

    isr_install();

    init_keyboard();

    uint64_t kernel_end = (uint64_t)&_bss_end;

    pmm_init(kernel_end + 0x100000, 0x10000000);
    init_vmm(boot_info);

    heap_init();

    tasking_init();
    create_task(launch_shell);

    kprint("System Online. Enabling Interrupts.\n");
    __asm__ __volatile__("sti");

    while(1) __asm__ __volatile__("hlt");
}
