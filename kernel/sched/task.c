#include "kernel/sched/task.h"
#include "kernel/mem/heap.h"
#include "drivers/screen.h"

task_t *current_task = 0;
task_t *ready_queue = 0;
uint32_t next_pid = 1;

extern void switch_task(uint64_t *old_rsp, uint64_t new_rsp);

void tasking_init() {
    current_task = (task_t*)kmalloc(sizeof(task_t));
    current_task->pid = next_pid++;
    current_task->rsp = 0;
    current_task->next = 0;

    ready_queue = current_task;
    kprint("[SCHED] - Multitasking Initialized.\n");
}

void create_task(void (*entry)()) {
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->pid = next_pid++;
    new_task->next = 0;

    uint64_t *stack = (uint64_t*)kmalloc(4096);
    uint64_t *top = stack + 512;

    // Forge the stack for switch_task:
    *(--top) = (uint64_t)entry;   // RIP
    *(--top) = 0x202;             // RFLAGS (IF bit set)
    *(--top) = 0;                 // RBX
    *(--top) = 0;                 // RBP
    *(--top) = 0;                 // R12
    *(--top) = 0;                 // R13
    *(--top) = 0;                 // R14
    *(--top) = 0;                 // R15

    new_task->rsp = (uint64_t)top;

    task_t *temp = ready_queue;
    while(temp->next) temp = temp->next;
    temp->next = new_task;
}

void schedule() {
    if (!current_task || !ready_queue) return;

    // Prevent re-entrant scheduling
    __asm__ __volatile__("cli");

    task_t *next = current_task->next;
    if (!next) next = ready_queue;

    if (next == current_task) {
        __asm__ __volatile__("sti");
        return;
    }

    task_t *prev = current_task;
    current_task = next;

    // Switch stacks
    switch_task(&(prev->rsp), current_task->rsp);

    // switch_task returns here for the new task
    __asm__ __volatile__("sti");
}
