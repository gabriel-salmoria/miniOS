#include "drivers/keyboard.h"
#include "kernel/cpu/isr.h"
#include "drivers/apic/ports.h"
#include "kernel/sched/task.h"

#define KBD_BUF_SIZE 256

static char kbd_buf[KBD_BUF_SIZE];
static volatile int kbd_head = 0;
static volatile int kbd_tail = 0;

const char sc_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', '?', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '?', '?',
    '?', ' '
};

static void keyboard_callback(registers_t *regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);

    if (scancode < 58) {
        char ascii = sc_ascii[scancode];
        if (ascii) {
            kbd_buf[kbd_head] = ascii;
            kbd_head = (kbd_head + 1) % KBD_BUF_SIZE;
            unblock_all();
            schedule();
        }
    }
}

void init_keyboard() {
    register_interrupt_handler(33, keyboard_callback);
}

char kbd_getchar() {
    while (kbd_head == kbd_tail) {
        block_task();
    }

    __asm__ __volatile__("cli");
    char c = kbd_buf[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBD_BUF_SIZE;
    __asm__ __volatile__("sti");

    return c;
}
