OUTPUT_FORMAT("elf32-xstormy16", "elf32-xstormy16", "elf32-xstormy16")
OUTPUT_ARCH(xstormy16)
ENTRY(_start)
GROUP( -lc -lgcc -lm -liberty)

INPUT( crti.o )
INPUT( crtbegin.o )
INPUT( crtend.o )
INPUT( crtn.o )

                                /*************************************/
                                /** memory region definition        **/
                                /*************************************/
__malloc_start = 0x7E00;
MEMORY
{
    RAM (w)    : ORIGIN = 0x00000, LENGTH = 0x7E00
    CHIP (r)   : ORIGIN = 0x07f00, LENGTH = 0x100
    LOROM (w)  : ORIGIN = 0x08000, LENGTH = 0x8000
    ROM (!w)   : ORIGIN = 0x08000, LENGTH = 0x78000
    BIOS (!w)  : ORIGIN = 0x8000+0x78000-0x0000, LENGTH = 0x0000
    BIVEC (!w) : ORIGIN = 0x00000-0x100, LENGTH = 0x100
    OPTION (r) : ORIGIN = 0x00000, LENGTH = 0x100
}

SECTIONS
{
                                /*************************************/
                                /** .data section                   **/
                                /*************************************/
    .data : AT ( __rdata ) {
        __data = .;
        *(.data_0)
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
        . = ALIGN(2);
    } > RAM =0
    _edata = .;
                                /*************************************/
                                /** .bss section                    **/
                                /*************************************/
    .bss : AT (LOADADDR(.data) + SIZEOF(.data)) {
        *(.dynbss)
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(2);
    } > RAM
    _end = .;
                                /*************************************/
                                /** define top of stack             **/
                                /*************************************/
    __stack = .;
                                /*************************************/
                                /** sections for target chip info   **/
                                /*************************************/
    .chip_info : {
        KEEP(*(.chip_info))
    } > CHIP =0
                                /*************************************/
                                /** reset/interrupt vectors at 8000 **/
                                /*************************************/
    .int_vec : {
        KEEP(*(.int_vec))
    } > ROM =0
                                /*************************************/
                                /** read only section               **/
                                /*************************************/
    .rodata : {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
    } > ROM =0
                                /*************************************/
                                /** construcrtors/destructors       **/
                                /*************************************/
    .ctors : {
        KEEP (*crti.o(.ctors))
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtn.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        KEEP (*crtn.o(.ctors))
    } > ROM =0
   .dtors : {
        KEEP (*crti.o(.dtors))
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtn.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        KEEP (*crtn.o(.dtors))
    } > ROM =0
                                /*************************************/
                                /** stab for the pointer to function**/
                                /*************************************/
    .plt : {
        *(.plt)
    } > ROM =0
                                /*************************************/
                                /** other information               **/
                                /*************************************/
    .jcr : {
        KEEP (*(.jcr))
    } > ROM =0
    .eh_frame : {
        KEEP (*(.eh_frame))
    } > ROM =0
    .gcc_except_table : {
        KEEP (*(.gcc_except_table))
    } > ROM =0
                                /*************************************/
                                /** .text section                   **/
                                /*************************************/
    .text : {
        *(.text)
        *(.text.*)
        *(.stub)
        *(.gnu.warning)
        *(.gnu.linkonce.t.*)
    } > ROM =0
                                /*************************************/
                                /** .init/.fini section             **/
                                /*************************************/
    .init : {
        KEEP (*crti.o(.init))
        KEEP (*crtbegin.o(.init))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtn.o) .init))
        KEEP (*crtend.o(.init))
        KEEP (*crtn.o(.init))
    } > ROM =0
    .fini : {
        KEEP (*crti.o(.fini))
        KEEP (*crtbegin.o(.fini))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtn.o) .fini))
        KEEP (*crtend.o(.fini))
        KEEP (*crtn.o(.fini))
    } > ROM =0
                                /*************************************/
                                /** external data                   **/
                                /*************************************/
    .ex_data : {
        *(.ex_data)
    } > ROM =0
                                /*************************************/
                                /** initialize value for .data      **/
                                /*************************************/
    .data_init (NOLOAD) : {
        __rdata = . ;
        . += SIZEOF(.data) ;
    } > ROM
                                /*************************************/
                                /** CG-ROM                          **/
                                /*************************************/
    .cg_rom ALIGN(256) : {
        KEEP(*(.cg_rom))
    } > ROM =0
                                /*************************************/
                                /** unused ROM area                 **/
                                /*************************************/
    __etext = .;
    __erom = 0x8000+0x78000-0x0000;
                                /*************************************/
                                /** Stab debugging sections         **/
                                /*************************************/
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }
                                /*************************************/
                                /** DWARF debug sections            **/
                                /*************************************/
    /* Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug 0 : { *(.debug) }
    .line 0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo 0 : { *(.debug_srcinfo) }
    .debug_sfnames 0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges 0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info 0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
    .debug_abbrev 0 : { *(.debug_abbrev) }
    .debug_line 0 : { *(.debug_line) }
    .debug_frame 0 : { *(.debug_frame) }
    .debug_str 0 : { *(.debug_str) }
    .debug_loc 0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
