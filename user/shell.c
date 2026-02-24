#include "user/shell.h"
#include "libc/string.h"

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

void user_print(char *str) {
    do_syscall(0, (uint64_t)str, 0, 0);
}

char user_getchar() {
    return (char)do_syscall(1, 0, 0, 0);
}

void launch_shell() {
    user_print("\n[ShitOS Shell]\n> ");

    char input[256];
    int idx = 0;

    while (1) {
        char c = user_getchar();

        if (c == '\n') {
            user_print("\n");
            input[idx] = '\0';

            if (strcmp(input, "clear") == 0) {
                // handle clear
            }

            idx = 0;
            user_print("> ");
        } else if (c == '\b') {
            if (idx > 0) idx--;
            // Handle backspace logic later via TTY system
        } else {
            if (idx < 255) {
                input[idx++] = c;
                char str[2] = {c, 0};
                user_print(str);
            }
        }
    }
}
