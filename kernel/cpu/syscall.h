#ifndef SYSCALL_H
#define SYSCALL_H

#include <types.h>

void syscall_init();
uint64_t syscall_handler(uint64_t id, uint64_t arg1, uint64_t arg2, uint64_t arg3);
void set_kernel_stack(uint64_t stack);

#endif
