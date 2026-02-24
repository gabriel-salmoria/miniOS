#include "kernel/cpu/syscall.h"
#include "kernel/cpu/msr.h"
#include "kernel/cpu/gdt.h"
#include "kernel/mem/heap.h"
#include "kernel/sched/task.h"

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
    if (id == 0 || id == 1) {
        int fd = arg1;
        uint8_t *buf = (uint8_t*)arg2;
        uint32_t count = arg3;

        if (fd < 0 || fd >= MAX_FD || !current_task->fd_table[fd]) return -1;
        vfs_node_t *node = current_task->fd_table[fd];

        if (id == 0 && node->write) return node->write(node, 0, count, buf);
        if (id == 1 && node->read) return node->read(node, 0, count, buf);
    }
    else if (id == 2) {
        exit_task();
        return 0;
    }
    else if (id == 3) {
        schedule();
        return 0;
    }
    else if (id == 4) { // sys_open
        const char *path = (const char*)arg1;
        vfs_node_t *node = vfs_open(path);
        if (!node) return -1;

        for (int i = 0; i < MAX_FD; i++) {
            if (!current_task->fd_table[i]) {
                current_task->fd_table[i] = node;
                return i;
            }
        }
        return -1;
    }
    else if (id == 5) { // sys_close
        int fd = arg1;
        if (fd < 0 || fd >= MAX_FD || !current_task->fd_table[fd]) return -1;

        vfs_node_t *node = current_task->fd_table[fd];
        if (node->close) node->close(node);

        current_task->fd_table[fd] = 0;
        return 0;
    }

    return -1;
}

// Add this below your gs_local declaration
void set_kernel_stack(uint64_t stack) {
    gs_local[1] = stack;    // For syscalls
    set_tss_rsp0(stack);    // For hardware interrupts
}
