#include "kernel/cpu/syscall.h"
#include "kernel/cpu/msr.h"
#include "kernel/cpu/gdt.h"
#include "drivers/screen.h"
#include "kernel/mem/heap.h"
#include "drivers/keyboard.h"

#define MSR_KERNEL_GS_BASE 0xC0000102

extern void syscall_entry();

// gs_local[0] = User RSP scratch, gs_local[1] = Dedicated Kernel RSP
static uint64_t gs_local[2];

void syscall_init() {
    uint64_t efer = rdmsr(IA32_EFER);
    wrmsr(IA32_EFER, efer | 1);

    // FIX: Must use KERNEL_DS (0x10) so sysret calculates 0x20 (User CS) and 0x18 (User DS)
    uint64_t star = ((uint64_t)KERNEL_DS << 48) | ((uint64_t)KERNEL_CS << 32);
    wrmsr(IA32_STAR, star);

    wrmsr(IA32_LSTAR, (uint64_t)syscall_entry);
    wrmsr(IA32_FMASK, 0x200);

    // Setup GS Base for swapgs
    gs_local[1] = (uint64_t)kmalloc(4096) + 4096;
    wrmsr(MSR_KERNEL_GS_BASE, (uint64_t)&gs_local);
}

uint64_t syscall_handler(uint64_t id, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    if (id == 0) {
        kprint((char*)arg1);
        return 0;
    } else if (id == 1) {
        return kbd_getchar();
    }
    return -1;
}

// Add this below your gs_local declaration
void set_kernel_stack(uint64_t stack) {
    gs_local[1] = stack;    // For syscalls
    set_tss_rsp0(stack);    // For hardware interrupts
}
