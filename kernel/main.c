#include "drivers/screen.h"
#include "kernel/cpu/isr.h"
#include "drivers/keyboard.h"
#include "kernel/fs/vfs.h"
#include "user/shell.h"
#include "kernel/mem/pmm.h"
#include "kernel/mem/vmm.h"
#include "kernel/mem/heap.h"
#include "kernel/sched/task.h"
#include "kernel/cpu/gdt.h"
#include "libc/string.h"
#include <shared_types.h>
#include "kernel/sched/timer.h"
#include "kernel/acpi/acpi.h"
#include "drivers/apic/apic.h"
#include "drivers/apic/ioapic.h"
#include "kernel/cpu/syscall.h"

extern uint64_t _bss_start;
extern uint64_t _bss_end;

static void init_bss() {
    uint64_t bss_size = (uint64_t)&_bss_end - (uint64_t)&_bss_start;
    memset(&_bss_start, 0, bss_size);
}

static void init_memory(boot_info_t *boot_info) {
    uint64_t kernel_end = (uint64_t)&_bss_end;
    pmm_init(kernel_end + 0x100000, 0x02000000);
    init_vmm();
    heap_init();

    vfs_init();
}

static void init_system(boot_info_t *boot_info) {
    init_gdt();
    isr_install();

    syscall_init();


    // Parse ACPI and initialize APIC instead of legacy PIC
    acpi_init(boot_info->rsdp);
    init_apic();
    init_ioapic();

    init_timer();
    init_keyboard();
}

__attribute__((ms_abi, section(".text.entry")))
void main(boot_info_t *boot_info) {
    init_bss();

    init_screen(boot_info);
    kprint("ShitOS 64-bit: Kernel Started\n");

    init_system(boot_info);
    init_memory(boot_info);

    tasking_init();
    create_user_task(launch_shell);

    kprint("System Online. Enabling Interrupts.\n");
    __asm__ __volatile__("sti");

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
