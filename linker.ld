ENTRY(main)
OUTPUT_FORMAT("binary")

SECTIONS {
    . = 0x100000;

    .text : ALIGN(4) {
        *(.text.entry)
        *(.text)
        *(.text.*)
    }
    .rodata : ALIGN(4) {
        *(.rodata)
        *(.rodata.*)
    }
    .data : ALIGN(4) {
        *(.data)
        *(.data.*)
    }
    .bss : ALIGN(4) {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.bss.*)
        _bss_end = .;
    }
    /DISCARD/ : {
        *(.eh_frame)
        *(.note*)
        *(.comment)
    }
}
