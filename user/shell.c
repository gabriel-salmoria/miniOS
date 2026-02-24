#include "user/shell.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "libc/string.h"

void launch_shell() {
    kprint("\n[ShitOS Shell]\n> ");

    char input[256];
    int idx = 0;

    while (1) {
        char c = kbd_getchar();

        if (c == '\n') {
            kprint("\n");
            input[idx] = '\0';

            if (strcmp(input, "clear") == 0) {
                // handle clear
            }

            idx = 0;
            kprint("> ");
        } else if (c == '\b') {
            if (idx > 0) {
                idx--;
                // Handle backspace visual removal here
            }
        } else {
            if (idx < 255) {
                input[idx++] = c;
                char str[2] = {c, 0};
                kprint(str);
            }
        }
    }
}
