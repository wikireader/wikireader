/*
 *  @(#) $Id: m68kelf.ld,v 1.3 2003/07/01 13:54:47 hiro Exp $
 */

OUTPUT_FORMAT("elf32-m68k","elf32-m68k","elf32-m68k")
OUTPUT_ARCH(m68k)

PROVIDE (__stack = 0) ;
PROVIDE (hardware_init_hook = 0) ;
PROVIDE (software_init_hook = 0) ;
PROVIDE (software_term_hook = 0) ;
SECTIONS
{
    .init :
    {
        KEEP (*(.init))
    }
    .text :
    {
        __text = . ;
        *(.text .stub .text.* .gnu.linkonce.t.*)

    }
    .fini :
    {
        KEEP (*(.fini))
    }
    _etext = . ;
    PROVIDE (etext = .) ;
    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }
    . = ALIGN(4) ;
    __idata_start = . ;
    .data : AT(__idata_start) 
    {
        __data_start = . ;
        *(.data)
    }
    .eh_frame : { KEEP (*(.eh_frame)) }
    .gcc_except_table : { *(.gcc_except_table) }
    .ctors :
    {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    }
    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    }
    __idata_end = __idata_start + SIZEOF(.data) ;
    _edata  = . ;
    PROVIDE (edata = .) ;
    . = ALIGN(4) ;
    __bss_start = . ;
    .bss :
    {
        *(.bss)
        *(COMMON)
    }
    _end = . ;
    __bss_end = . ;
    PROVIDE (end = .) ;
    .comment        0 : { *(.comment) }
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
}
