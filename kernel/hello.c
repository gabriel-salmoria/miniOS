#include "../drivers/screen.h"
#include "../include/isr.h"
#include "../drivers/keyboard.h"

int strcmp(char *s1, char *s2);
void user_input(char *input);

void main() {
    clear_screen();
    kprint_at("ShitOS 32-bit Kernel\n", 0, 0);
    kprint("Type 'halt' to exit or 'clear' to clear.\n> ");

    isr_install();
    init_keyboard();
}

// 3. Helper Functions Implementation
int strcmp(char *s1, char *s2) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

void user_input(char *input) {
    if (strcmp(input, "halt") == 0) {
        kprint("Stopping the CPU. Bye!\n");
        __asm__ __volatile__("hlt");
    }
    else if (strcmp(input, "clear") == 0) {
        clear_screen();
        kprint_at("ShitOS Shell > ", 0, 0);
    }
    else {
        kprint("You said: ");
        kprint(input);
        kprint("\n> ");
    }
}
