#ifndef ISR_H
#define ISR_H

#include <types.h>

typedef struct {
    // Registers pushed by us in assembly
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

    // Interrupt info pushed by assembly
    uint64_t int_no, err_code;

    // Pushed by the processor automatically in 64-bit mode
    uint64_t rip, cs, rflags, rsp, ss;
} registers_t;

typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(uint8_t n, isr_t handler);
void isr_install();
#endif
