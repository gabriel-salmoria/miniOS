ENTRY(main)
OUTPUT_FORMAT("binary")

SECTIONS {
    . = 0x100000; /* CHANGED: 1MB mark */

    .text : ALIGN(4) {
        *(.text)
    }
    .rodata : ALIGN(4) {
        *(.rodata)
    }
    .data : ALIGN(4) {
        *(.data)
    }
    .bss : ALIGN(4) {
        *(COMMON)
        *(.bss)
    }
    end = .;
    /DISCARD/ : {
        *(.eh_frame)
        *(.note*)
        *(.comment)
    }
}
