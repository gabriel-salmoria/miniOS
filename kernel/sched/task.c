#include "kernel/sched/task.h"
#include "kernel/mem/heap.h"
#include "kernel/cpu/syscall.h"

task_t * volatile current_task = 0;
task_t * volatile ready_queue = 0;
uint32_t next_pid = 1;

static task_t main_task;
extern void switch_task(uint64_t *old_rsp, uint64_t new_rsp);

void tasking_init() {
    current_task = &main_task;
    current_task->pid = next_pid++;
    current_task->state = TASK_READY;
    current_task->rsp = 0;
    current_task->next = 0;
    current_task->kernel_stack_top = 0;
    ready_queue = current_task;
}

void create_task(void (*entry)()) {
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));

    // NEW: Stop the system if the heap is broken
    if (!new_task) {
        while(1) __asm__ __volatile__("hlt");
    }

    new_task->pid = next_pid++;
    new_task->next = 0;

    uint64_t *kernel_stack = (uint64_t*)kmalloc(4096);
    uint64_t k_top = (((uint64_t)kernel_stack + 4096) & -16ULL);

    new_task->kernel_stack_top = k_top; // Save it for context switches

    uint64_t *ptr = (uint64_t*)k_top;

    // Forge the stack frame
    *(--ptr) = 0;                 // CRITICAL: Dummy Return Address for ABI Alignment
    *(--ptr) = (uint64_t)entry;   // RIP for switch_task's 'ret'
    *(--ptr) = 0x202;             // RFLAGS
    *(--ptr) = 0;                 // rbx
    *(--ptr) = 0;                 // rbp
    *(--ptr) = 0;                 // r12
    *(--ptr) = 0;                 // r13
    *(--ptr) = 0;                 // r14
    *(--ptr) = 0;                 // r15

    new_task->rsp = (uint64_t)ptr;

    task_t *temp = ready_queue;
    while(temp->next) temp = temp->next;
    temp->next = new_task;
}

extern void jump_usermode(uint64_t entry, uint64_t user_rsp);

void create_user_task(void (*entry)()) {
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->pid = next_pid++;
    new_task->state = TASK_READY;
    new_task->next = 0;

    new_task->fd_table[0] = vfs_stdin;
    new_task->fd_table[1] = vfs_stdout;
    for (int i = 2; i < MAX_FD; i++) new_task->fd_table[i] = 0;

    // Kernel stack for interrupt handling
    uint64_t *kernel_stack = (uint64_t*)kmalloc(4096);
    uint64_t *k_top = (uint64_t*)(((uint64_t)kernel_stack + 4096) & -16ULL);

    // User stack for Ring 3 execution
    uint64_t *user_stack = (uint64_t*)kmalloc(4096);
    uint64_t user_rsp = ((uint64_t)user_stack + 4096) & -16ULL;

    *(--k_top) = 0;
    *(--k_top) = (uint64_t)jump_usermode; // switch_task 'ret' jumps here
    *(--k_top) = 0x202;                   // RFLAGS
    *(--k_top) = 0;                       // rbx
    *(--k_top) = 0;                       // rbp
    *(--k_top) = 0;                       // r12
    *(--k_top) = 0;                       // r13
    *(--k_top) = (uint64_t)user_rsp;      // r14 (passed to rsi in jump_usermode)
    *(--k_top) = (uint64_t)entry;         // r15 (passed to rdi in jump_usermode)

    new_task->rsp = (uint64_t)k_top;

    task_t *temp = ready_queue;
    while(temp->next) temp = temp->next;
    temp->next = new_task;
}

void block_task() {
    if (current_task) current_task->state = TASK_BLOCKED;
    schedule();
}

void unblock_all() {
    task_t *temp = ready_queue;
    while (temp) {
        temp->state = TASK_READY;
        temp = temp->next;
    }
}

void schedule() {
    if (!current_task || !ready_queue) return;
    __asm__ __volatile__("cli");

    task_t *next = current_task->next;

    while (1) {
        if (!next) next = ready_queue;
        if (next->state == TASK_READY) break;

        if (next == current_task) {
            if (current_task->state == TASK_BLOCKED) {
                // All tasks blocked. Sleep until next interrupt.
                __asm__ __volatile__("sti\n\thlt\n\tcli");
                next = current_task->next;
                continue;
            }
            break;
        }
        next = next->next;
    }

    if (next == current_task) {
        __asm__ __volatile__("sti");
        return;
    }

    task_t *prev = current_task;
    current_task = next;

    // NEW: Update hardware pointers to this task's kernel stack
    if (current_task->kernel_stack_top != 0) {
        set_kernel_stack(current_task->kernel_stack_top);
    }
    switch_task(&(prev->rsp), current_task->rsp);

    __asm__ __volatile__("sti");
}
