#include "kernel/sched/task.h"
#include "kernel/mem/heap.h"
#include "drivers/screen.h"
#include "kernel/cpu/isr.h"

task_t *current_task = 0;
task_t *ready_queue = 0;
uint32_t next_pid = 1;

// Imported from switch.asm
extern void switch_task(uint32_t *old_esp, uint32_t new_esp);

void tasking_init() {
    // 1. Create a struct for the CURRENTLY running kernel (Main Task)
    // We don't need to forge a stack; we are already running on it.
    current_task = (task_t*)kmalloc(sizeof(task_t));
    current_task->pid = next_pid++;
    current_task->esp = 0; // Will be set by switch_task later
    current_task->next = 0;

    ready_queue = current_task;

    kprint("Multitasking Initialized.\n");
}

void create_task(void (*entry)()) {
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->pid = next_pid++;
    new_task->next = 0;

    uint32_t *stack = (uint32_t*)kmalloc(4096);
    uint32_t *top = stack + 1024;

    // --- FORGE THE STACK ---
    // Layout MUST match the pops in switch.asm:
    // [EBP] [EDI] [ESI] [EBX] [EFLAGS] [RET_ADDR]

    // A. Return Address
    *(--top) = (uint32_t)entry;

    // B. EFLAGS (Crucial Fix!)
    // 0x202 = Interrupts Enabled.
    *(--top) = 0x202;

    // C. General Registers
    *(--top) = 0; // ebx
    *(--top) = 0; // esi
    *(--top) = 0; // edi
    *(--top) = 0; // ebp

    new_task->esp = (uint32_t)top;

    // Add to queue logic...
    task_t *temp = ready_queue;
    while(temp->next) temp = temp->next;
    temp->next = new_task;
}

void schedule() {
    if (!current_task) return;

    // Simple Round Robin
    task_t *next = current_task->next;
    if (!next) next = ready_queue; // Loop back to start

    if (next == current_task) return; // Only one task? Don't switch.

    task_t *prev = current_task;
    current_task = next;

    // The Magic Switch
    // This saves 'prev' state and restores 'next' state.
    switch_task(&(prev->esp), current_task->esp);
}
