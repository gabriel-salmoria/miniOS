#include "user/shell.h"
#include "libc/syscall.h"
#include "libc/string.h"

void user_print(char *str) {
    // Write string to FD 1 (stdout)
    write(1, str, strlen(str));
}

char user_getchar() {
    char c = 0;
    // Read 1 byte from FD 0 (stdin)
    read(0, &c, 1);
    return c;
}

void launch_shell() {
    user_print("\n[ShitOS Shell]\n> ");

    char input[256];
    int idx = 0;

    while (1) {
        char c = user_getchar();

        if (c == '\n') {
            user_print("\n");
            input[idx] = '\0';

            if (strcmp(input, "clear") == 0) {
                // handle clear
            }

            idx = 0;
            user_print("> ");
        } else if (c == '\b') {
            if (idx > 0) idx--;
            // Handle backspace logic later via TTY system
        } else if (c != 0) { // Ensure we don't print null bytes
            if (idx < 255) {
                input[idx++] = c;
                char str[2] = {c, 0};
                user_print(str);
            }
        }
    }
}
