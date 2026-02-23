#ifndef TIMER_H
#define TIMER_H

#include <types.h>
#include "kernel/cpu/isr.h"

void timer_callback(registers_t *regs);
void init_timer();

#endif // TIMER_H
