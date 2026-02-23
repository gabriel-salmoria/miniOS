#include "kernel/sched/task.h"
#include "kernel/mem/heap.h"
#include "drivers/screen.h"
#include "libc/string.h"

task_t * current_task = 0; // Actual definition
task_t * ready_queue = 0;
uint32_t next_pid = 1;

extern void switch_task(uint64_t *old_rsp, uint64_t new_rsp);

// Define a static task for the kernel main so current_task is never NULL
static task_t main_task;

void tasking_init() {
    current_task = &main_task;
    current_task->pid = next_pid++;
    current_task->rsp = 0;
    current_task->next = 0;
    ready_queue = current_task;
    kprint("[SCHED] - Multitasking Initialized.\n");
}

void create_task(void (*entry)()) {
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));

    // NEW: Stop the system if the heap is broken
    if (!new_task) {
        kprint("\n[PANIC] kmalloc failed in create_task!\n");
        while(1) __asm__ __volatile__("hlt");
    }

    new_task->pid = next_pid++;
    new_task->next = 0;

    uint64_t *stack = (uint64_t*)kmalloc(4096);
    // Align top to 16 bytes
    uint64_t *top = (uint64_t*)(((uint64_t)stack + 4096) & -16ULL);

    // Forge the stack frame
    *(--top) = 0;                 // CRITICAL: Dummy Return Address for ABI Alignment
    *(--top) = (uint64_t)entry;   // RIP for switch_task's 'ret'
    *(--top) = 0x202;             // RFLAGS
    *(--top) = 0;                 // rbx
    *(--top) = 0;                 // rbp
    *(--top) = 0;                 // r12
    *(--top) = 0;                 // r13
    *(--top) = 0;                 // r14
    *(--top) = 0;                 // r15

    new_task->rsp = (uint64_t)top;

    task_t *temp = ready_queue;
    while(temp->next) temp = temp->next;
    temp->next = new_task;
}

void schedule() {
    if (!current_task || !ready_queue) return;

    task_t *next = current_task->next;
    if (!next) next = ready_queue;
    if (next == current_task) return;

    __asm__ __volatile__("cli");

    task_t *prev = current_task;
    current_task = next;

    switch_task(&(prev->rsp), current_task->rsp);

    // This only runs when THIS task is resumed
    __asm__ __volatile__("sti");
}
