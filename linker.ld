ENTRY(main)
OUTPUT_FORMAT("binary")

SECTIONS {
    . = 0x100000;

    .text : ALIGN(4) {
        *(.text)
        *(.text.*) /* Add this to catch all functions */
    }
    .rodata : ALIGN(4) {
        *(.rodata)
        *(.rodata.*) /* Catch read-only data subsections */
    }
    .data : ALIGN(4) {
        *(.data)
        *(.data.*)
    }
    .bss : ALIGN(4) {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    end = .;
    /DISCARD/ : {
        *(.eh_frame)
        *(.note*)
        *(.comment)
    }
}
