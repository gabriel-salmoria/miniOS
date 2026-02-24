#ifndef TASK_H
#define TASK_H

#include <types.h>

typedef enum {
    TASK_READY,
    TASK_BLOCKED
} task_state_t;

typedef struct task {
    uint64_t rsp;
    uint64_t cr3;
    uint32_t pid;
    task_state_t state;
    struct task *next;
} task_t;

extern task_t * volatile current_task;
extern task_t * volatile ready_queue;

void tasking_init();
void create_task(void (*entry)());
void schedule();
void block_task();
void unblock_all();

#endif
