#include "libc/syscall.h"

#include <types.h>

static inline uint64_t do_syscall(uint64_t id, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    register uint64_t rax __asm__("rax") = id;
    register uint64_t rdi __asm__("rdi") = arg1;
    register uint64_t rsi __asm__("rsi") = arg2;
    register uint64_t rdx __asm__("rdx") = arg3;

    __asm__ __volatile__(
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx)
        : "rcx", "r11", "memory"
    );

    return rax;
}

int write(int fd, const void *buf, uint32_t count) {
    return (int)do_syscall(0, (uint64_t)fd, (uint64_t)buf, (uint64_t)count);
}

int read(int fd, void *buf, uint32_t count) {
    return (int)do_syscall(1, (uint64_t)fd, (uint64_t)buf, (uint64_t)count);
}

void exit(int status) {
    do_syscall(2, (uint64_t)status, 0, 0);
    while(1); // Catch in case of failure
}

void yield() {
    do_syscall(3, 0, 0, 0);
}

int open(const char *path) {
    return (int)do_syscall(4, (uint64_t)path, 0, 0);
}

int close(int fd) {
    return (int)do_syscall(5, (uint64_t)fd, 0, 0);
}
