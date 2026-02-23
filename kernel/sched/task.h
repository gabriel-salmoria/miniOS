#ifndef TASK_H
#define TASK_H

#include <types.h>

typedef struct task {
    uint64_t rsp;          // Stack Pointer (64-bit)
    uint64_t cr3;          // Page Directory (64-bit)
    uint32_t pid;
    struct task *next;
} task_t;

extern task_t * current_task;
extern task_t * ready_queue;

void tasking_init();
void create_task(void (*entry)());
void schedule();

#endif
