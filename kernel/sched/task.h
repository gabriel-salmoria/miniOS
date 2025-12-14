#ifndef TASK_H
#define TASK_H

#include <types.h>
#include "kernel/mem/vmm.h"

typedef struct task {
    uint32_t esp;          // Stack Pointer (Points to the saved registers on the stack)
    uint32_t cr3;          // Page Directory (Physical address for virtual memory)
    uint32_t pid;          // Task ID (1, 2, 3...)
    struct task *next;     // Linked List pointer to the next task
} task_t;

// Global pointer to the currently running task
extern task_t *current_task;

void tasking_init();
void create_task(void (*entry)());
void schedule();

#endif
