/*
 *  @(#) $Id: shlelf.ld,v 1.3 2003/12/04 14:14:39 honda Exp $
 */

OUTPUT_FORMAT("elf32-shl","elf32-shl","elf32-shl")
OUTPUT_ARCH(sh)

PROVIDE(_hardware_init_hook = 0);
PROVIDE(_software_init_hook = 0);
PROVIDE(_software_term_hook = 0);

SECTIONS
{
    .text :
    {
        __text = . ;
        *(.text .stub .text.* .gnu.linkonce.t.*)
    }
    .init :
    {
        KEEP (*(.init))
    } = 0
    .fini :
    {
        KEEP (*(.fini))
    } = 0
    _etext = . ;
    PROVIDE (etext = .) ;
   .eh_frame_hdr : { *(.eh_frame_hdr) }
    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }
    . = ALIGN(4);
    __idata_start = . ;
    .data   :  AT(__idata_start) 
    {
        __data_start = . ;
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    }
    __idata_end = __idata_start + SIZEOF(.data);
    .eh_frame       : { KEEP (*(.eh_frame)) }
    .gcc_except_table   : { *(.gcc_except_table) }
    .ctors :
    {
        ___ctors = .;
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        ___ctors_end = .;
    }
    .dtors :
    {
        ___dtors = .;
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        ___dtors_end = .;
    }
    .jcr   : { KEEP (*(.jcr)) }
    .got   : { *(.got.plt) *(.got) }
    _edata  =  . ;
    PROVIDE (edata = .);
    . = ALIGN(4);
    __bss_start = . ;
    .bss       :
    {

      *(.bss)
      *(COMMON)
    }
    . = ALIGN(4);
    _end = .		;
    PROVIDE (end = .)	;
    .comment 0 : { *(.comment) }	
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
}

