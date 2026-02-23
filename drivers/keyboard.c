#include "drivers/keyboard.h"
#include "drivers/screen.h"
#include "drivers/ports.h"
#include "kernel/cpu/isr.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define MAX_BUFFER 256

static char key_buffer[MAX_BUFFER];
static int buffer_index = 0;
static int input_complete = 0; // Flag: 1 = Enter pressed, data ready

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

static void keyboard_callback(registers_t* regs) {
    (void)regs;
    uint8_t scancode = inb(0x60);

    // If top bit is set, it's a key release (break code)
    if (scancode & 0x80) return;

    if (scancode == BACKSPACE) {
        if (buffer_index > 0) {
            kprint_backspace();
            buffer_index--;
            key_buffer[buffer_index] = '\0';
        }
    } else if (scancode == ENTER) {
        kprint("\n");
        input_complete = 1;
    } else {
        // Broaden the check to include more of the sc_ascii table
        if (scancode >= sizeof(sc_ascii)) return;

        char letter = sc_ascii[scancode];
        if (letter == '?') return; // Ignore unknown keys

        char str[2] = {letter, '\0'};
        kprint(str);

        if (buffer_index < MAX_BUFFER - 1) {
            key_buffer[buffer_index++] = letter;
            key_buffer[buffer_index] = '\0';
        }
    }
}

void init_keyboard() {
    register_interrupt_handler(33, keyboard_callback);
    key_buffer[0] = '\0';
    buffer_index = 0;
    input_complete = 0;
}

// --- API ---

int is_input_complete() {
    return input_complete;
}

void get_keyboard_input(char *target_buffer) {
    // Copy buffer
    int i = 0;
    while (key_buffer[i] != '\0') {
        target_buffer[i] = key_buffer[i];
        i++;
    }
    target_buffer[i] = '\0';

    // Reset
    key_buffer[0] = '\0';
    buffer_index = 0;
    input_complete = 0;
}
