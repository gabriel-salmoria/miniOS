#ifndef TASK_H
#define TASK_H

#include <types.h>
#include "kernel/fs/vfs.h"


#define MAX_FD 16

typedef enum {
    TASK_READY,
    TASK_BLOCKED
} task_state_t;


typedef struct {
    vfs_node_t *node;
    uint64_t offset;
} file_descriptor_t;

typedef struct task {
    uint64_t rsp;
    uint64_t kernel_stack_top;
    uint64_t cr3;
    uint32_t pid;
    task_state_t state;
    file_descriptor_t fd_table[MAX_FD];
    struct task *next;
} task_t;
extern task_t * volatile current_task;


extern task_t * volatile ready_queue;

void tasking_init();
void create_task(void (*entry)());
void create_user_task(void (*entry)());
void schedule();
void block_task();
void unblock_all();
void exit_task();

#endif
