#include "kernel/sched/timer.h"
#include "kernel/sched/task.h"
#include "drivers/apic/apic.h"

#define LAPIC_TIMER_DIV  0x03E0
#define LAPIC_TIMER_INIT 0x0380
#define LAPIC_TIMER_LVT  0x0320

void timer_callback(registers_t *regs) {
    (void)regs;
    schedule();
}

void init_timer() {
    // Vector 32 | Periodic Mode (bit 17)
    apic_write(LAPIC_TIMER_LVT, 32 | 0x20000);

    // Divide by 16
    apic_write(LAPIC_TIMER_DIV, 0x03);

    // Lower this value to increase tick rate (e.g., 100,000 or 1,000,000)
    apic_write(LAPIC_TIMER_INIT, 1000000);

    register_interrupt_handler(32, timer_callback);
}
