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
    uint8_t scancode = inb(0x60);

    // If previous command wasn't handled, ignore new input (or implement a ring buffer later)
    if (input_complete) return;

    if (scancode & 0x80) return; // Ignore break codes

    if (scancode == BACKSPACE) {
        if (buffer_index > 0) {
            // Visual Update
            int offset = get_cursor_offset() - 2;
            int row = get_offset_row(offset);
            int col = get_offset_col(offset);
            print_char(' ', col, row, WHITE_ON_BLACK);
            set_cursor_offset(offset);

            // Buffer Update
            buffer_index--;
            key_buffer[buffer_index] = '\0';
        }
    }
    else if (scancode == ENTER) {
        kprint("\n");
        input_complete = 1; // Notify Consumer
    }
    else {
        if (scancode > 57) return;

        char letter = sc_ascii[scancode];
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
