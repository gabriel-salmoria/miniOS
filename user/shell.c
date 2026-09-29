#include "user/shell.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "libc/string.h"

void launch_shell() {
    kprint("\n[miniOS Shell - Type 'halt' or 'clear']\n> ");

    char input_buffer[256];

    while (1) {
        if (is_input_complete()) {
            get_keyboard_input(input_buffer);

            if (strcmp(input_buffer, "halt") == 0) {
                kprint("Stopping CPU. Bye!\n");
                __asm__ __volatile__("hlt");
            }
            else if (strcmp(input_buffer, "clear") == 0) {
                clear_screen();
                kprint("> ");
            }
            else if (strcmp(input_buffer, "help") == 0) {
                kprint("Commands: halt, clear, help\n> ");
            }
            else {
                kprint("Unknown command: ");
                kprint(input_buffer);
                kprint("\n> ");
            }
        }
    }
}
