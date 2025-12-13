#include "keyboard.h"
#include "ports.h"
#include "screen.h"
#include "../include/isr.h"

#define BACKSPACE 0x0E
#define ENTER 0x1C
#define MAX_BUFFER 128

static char key_buffer[MAX_BUFFER];
static int buffer_index = 0;

// Forward declaration of the kernel function we will call
extern void user_input(char *input);

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
    uint8_t scancode = port_byte_in(0x60);

    // Ignore key releases (highest bit set)
    if (scancode & 0x80) return;

    if (scancode == BACKSPACE) {
        if (buffer_index > 0) {
            // Visual Backspace
            int offset = get_cursor_offset() - 2;
            int row = get_offset_row(offset);
            int col = get_offset_col(offset);
            print_char(' ', col, row, WHITE_ON_BLACK);
            set_cursor_offset(offset);

            // Buffer Backspace
            buffer_index--;
            key_buffer[buffer_index] = '\0';
        }
    }
    else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); // Call kernel
        key_buffer[0] = '\0';   // Reset buffer
        buffer_index = 0;
    }
    else {
        // Normal Character
        if (scancode > 57) return; // Out of bounds

        char letter = sc_ascii[scancode];
        char str[2] = {letter, '\0'};
        kprint(str);

        // Add to buffer
        if (buffer_index < MAX_BUFFER - 1) {
            key_buffer[buffer_index++] = letter;
            key_buffer[buffer_index] = '\0';
        }
    }
}

void init_keyboard() {
    register_interrupt_handler(33, keyboard_callback);
    key_buffer[0] = '\0';
}
