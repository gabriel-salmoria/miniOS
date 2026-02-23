OUTPUT_FORMAT(pei-x86-64)

SECTIONS
{
  . = SIZEOF_HEADERS;
  . = ALIGN(__section_alignment__);

  .text __image_base__ + ( __section_alignment__ < 0x1000 ? . : __section_alignment__ ) :
  {
    KEEP(*(.assert_marker))
    KEEP(*(.assert_value))
    KEEP(*(.assert_msg))
    KEEP(*(SORT_NONE(.init)))
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    . = ALIGN(16);
    KEEP(*(SORT_NONE(.fini)))
    PROVIDE(etext = .);
  }

  .init_array ALIGN(8) :
  {
    __init_array_start = .;
    KEEP(*(.init_array))
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.ctors))
    KEEP(*(SORT_BY_NAME(.ctors.*)))
    __init_array_end = .;
  }

  .fini_array ALIGN(8) :
  {
    __fini_array_start = .;
    KEEP(*(.fini_array))
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.dtors))
    KEEP(*(SORT_BY_NAME(.dtors.*)))
    __fini_array_end = .;
  }

  .rdata BLOCK(__section_alignment__) :
  {
    *(.rdata)
    *(.rdata.*)
  }

  .data BLOCK(__section_alignment__) :
  {
    __data_start__ = .;
    *(.data)
    *(.data.*)
    __data_end__ = .;
  }

  .bss BLOCK(__section_alignment__) :
  {
    __bss_start__ = .;
    *(.bss)
    *(COMMON)
    __bss_end__ = .;
  }

  .eh_frame BLOCK(__section_alignment__) : { KEEP(*(.eh_frame*)) }
  .pdata BLOCK(__section_alignment__) : { KEEP(*(.pdata*)) }
  .xdata BLOCK(__section_alignment__) : { KEEP(*(.xdata*)) }

  .tls BLOCK(__section_alignment__) :
  {
    ___tls_start__ = .;
    *(.tls)
    *(.tls.*)
    ___tls_end__ = .;
  }

  .rsrc BLOCK(__section_alignment__) : SUBALIGN(4)
  {
    *(.rsrc)
    *(.rsrc.*)
  }

  .reloc BLOCK(__section_alignment__) : { *(.reloc) }

  .debug_aranges     BLOCK(__section_alignment__) : { *(.debug_aranges) }
  .debug_info        BLOCK(__section_alignment__) : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev      BLOCK(__section_alignment__) : { *(.debug_abbrev) }
  .debug_line        BLOCK(__section_alignment__) : { *(.debug_line) }
  .debug_frame       BLOCK(__section_alignment__) : { *(.debug_frame*) }
  .debug_str         BLOCK(__section_alignment__) : { *(.debug_str) }
  .debug_loc         BLOCK(__section_alignment__) : { *(.debug_loc) }
  .debug_ranges      BLOCK(__section_alignment__) : { *(.debug_ranges) }
  .debug_addr        BLOCK(__section_alignment__) : { *(.debug_addr) }
  .debug_line_str    BLOCK(__section_alignment__) : { *(.debug_line_str) }
  .debug_loclists    BLOCK(__section_alignment__) : { *(.debug_loclists) }
  .debug_rnglists    BLOCK(__section_alignment__) : { *(.debug_rnglists) }
  .debug_str_offsets BLOCK(__section_alignment__) : { *(.debug_str_offsets) }
  .debug_names       BLOCK(__section_alignment__) : { *(.debug_names) }

  .endjunk BLOCK(__section_alignment__) :
  {
    PROVIDE(end = .);
    PROVIDE(_end = .);
    __end__ = .;
  }

  /DISCARD/ :
  {
    *(.drectve)
    *(.note.GNU-stack)
    *(.gnu.lto_*)
    *(.comment)
  }
}
