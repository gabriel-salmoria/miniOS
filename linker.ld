ENTRY(main)
OUTPUT_FORMAT("binary")

SECTIONS {
    /* We load the kernel at 0x1000 */
    . = 0x1000;

    /* Code section */
    .text : ALIGN(4) {
        *(.text)
    }

    /* Read-only data (strings, constants) */
    .rodata : ALIGN(4) {
        *(.rodata)
    }

    /* Read-write data (global variables) */
    .data : ALIGN(4) {
        *(.data)
    }

    /* Uninitialized data (zero-filled) */
    .bss : ALIGN(4) {
        *(COMMON)
        *(.bss)
    }

    /* DISCARD EVERYTHING ELSE (Metadata, debug info, etc.) */
    /DISCARD/ : {
        *(.eh_frame)
        *(.note*)
        *(.comment)
    }
}
