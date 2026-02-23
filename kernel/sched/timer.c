#include "kernel/sched/timer.h"
#include "kernel/sched/task.h"
#include "drivers/ports.h"

void timer_callback(registers_t *regs) {
    (void)regs;
    schedule();
}

void init_timer() {
    // 1. Program the PIT (Programmable Interval Timer) to ~100Hz
    uint32_t divisor = 1193180 / 100;

    // Command port 0x43: Channel 0, Lobyte/Hibyte, Square Wave Mode
    outb(0x43, 0x36);

    // Data port 0x40: Send the divisor
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    // 2. Register the handler
    register_interrupt_handler(32, timer_callback);
}
