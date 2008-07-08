MEMORY
{
    reset : ORIGIN = 0x00000000, LENGTH = 32
    ext_flash : ORIGIN = 0x00000020, LENGTH = 8388576
    ext_ram : ORIGIN = 0x00800000, LENGTH = 1048576
    onchip_ram_64_kbytes : ORIGIN = 0x00900000, LENGTH = 65536
    sdram_UNUSED : ORIGIN = 0x01000000, LENGTH = 32
    sdram : ORIGIN = 0x01000020, LENGTH = 16777184
}

    /* Define symbols for each memory base-address */
 __alt_mem_ext_flash = 0x00000000 ;
 __alt_mem_ext_ram = 0x00800000 ;
 __alt_mem_onchip_ram_64_kbytes = 0x00900000 ;
 __alt_mem_sdram = 0x01000000 ;

PROVIDE(_hardware_init_hook = 0);
PROVIDE(_software_init_hook = 0);
PROVIDE(software_term_hook = 0);
PROVIDE( __alt_data_end = 0x02000000 );

OUTPUT_FORMAT( "elf32-littlenios2",
               "elf32-littlenios2",
               "elf32-littlenios2" )
OUTPUT_ARCH( nios2 )
ENTRY( _start )

/* Do we need any of these for elf?
   __DYNAMIC = 0;
 */

SECTIONS
{
    .entry :
    {
        KEEP (*(.entry))
    } > reset

    .exceptions :
    {
        . = ALIGN(0x20);
        *(.irq)
        KEEP (*(.exceptions));
    } > sdram

    .text :
    {
        /*
         * All code sections are merged into the text output section, along with
         * the read only data sections.
         *
         */

        PROVIDE (stext = ABSOLUTE(.));

        *(.interp)
        *(.hash)
        *(.dynsym)
        *(.dynstr)
        *(.gnu.version)
        *(.gnu.version_d)
        *(.gnu.version_r)
        *(.rel.init)
        *(.rela.init)
        *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)
        *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
        *(.rel.fini)
        *(.rela.fini)
        *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)
        *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
        *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
        *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
        *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)
        *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
        *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)
        *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)
        *(.rel.ctors)
        *(.rela.ctors)
        *(.rel.dtors)
        *(.rela.dtors)
        *(.rel.got)
        *(.rela.got)
        *(.rel.sdata .rel.sdata.* .rel.gnu.linkonce.s.*)
        *(.rela.sdata .rela.sdata.* .rela.gnu.linkonce.s.*)
        *(.rel.sbss .rel.sbss.* .rel.gnu.linkonce.sb.*)
        *(.rela.sbss .rela.sbss.* .rela.gnu.linkonce.sb.*)
        *(.rel.sdata2 .rel.sdata2.* .rel.gnu.linkonce.s2.*)
        *(.rela.sdata2 .rela.sdata2.* .rela.gnu.linkonce.s2.*)
        *(.rel.sbss2 .rel.sbss2.* .rel.gnu.linkonce.sb2.*)
        *(.rela.sbss2 .rela.sbss2.* .rela.gnu.linkonce.sb2.*)
        *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)
        *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)
        *(.rel.plt)
        *(.rela.plt)

        KEEP (*(.init))
        *(.plt)
        *(.text .stub .text.* .gnu.linkonce.t.*)

        /* .gnu.warning sections are handled specially by elf32.em.  */

        *(.gnu.warning.*)
        KEEP (*(.fini))
        PROVIDE (__etext = ABSOLUTE(.));
        PROVIDE (_etext = ABSOLUTE(.));
        PROVIDE (etext = ABSOLUTE(.));

        *(.eh_frame_hdr)
        /* Ensure the __preinit_array_start label is properly aligned.  We
           could instead move the label definition inside the section, but
           the linker would then create the section even if it turns out to
           be empty, which isn't pretty.  */
        . = ALIGN(32 / 8);
        PROVIDE (__preinit_array_start = ABSOLUTE(.));
        *(.preinit_array)
        PROVIDE (__preinit_array_end = ABSOLUTE(.));
        PROVIDE (__init_array_start = ABSOLUTE(.));
        *(.init_array)
        PROVIDE (__init_array_end = ABSOLUTE(.));
        PROVIDE (__fini_array_start = ABSOLUTE(.));
        *(.fini_array)
        PROVIDE (__fini_array_end = ABSOLUTE(.));
        SORT(CONSTRUCTORS)
        KEEP (*(.eh_frame))
        *(.gcc_except_table)
        *(.dynamic)

        /* gcc normally uses crtbegin.o to find the start of
           the constructors instead nios2 use crtdotors.o,
           so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtdotors.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtdotors.o
           is in.  */
        KEEP (*crtdotors.o(.ctors))
        /* We don't want to include the .ctor section from
           from the crtendtors (gcc normally uses crtend.o)
           file until after the sorted ctors.
           The .ctor section from the crtendtors file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtendtors.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        KEEP (*crtdotors.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtendtors.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        KEEP (*(.jcr))
    } > sdram =0x0001883a /* NOP on Nios2 */

    .rodata :
    {
        . = ALIGN(32 / 8);
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.rodata1)
    } > sdram

    .rwdata :
    {
        . = ALIGN(32 / 8);
        *(.got.plt) *(.got)
        *(.data1)
        *(.data .data.* .gnu.linkonce.d.*)

        _gp = ABSOLUTE(. + 0x7ff0);
        PROVIDE(gp = _gp);

        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)

        . = ALIGN(32 / 8);
        _edata = ABSOLUTE(.);
        PROVIDE (edata = ABSOLUTE(.));
    } > sdram

    .bss :
    {
        __bss_start = ABSOLUTE(.);
        PROVIDE (__sbss_start = ABSOLUTE(.));
        PROVIDE (___sbss_start = ABSOLUTE(.));

        *(.dynsbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
        *(.scommon)

        PROVIDE (__sbss_end = ABSOLUTE(.));
        PROVIDE (___sbss_end = ABSOLUTE(.));

        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)

        . = ALIGN(32 / 8);
        __bss_end = ABSOLUTE(.);
    } > sdram

    /*
     * One output section for each of the available partitions. These are not
     * used by default, but can be used by users applications using the .section
     * directive.
     *
     * The memory partition used for the heap is treated in  special way, i.e. a
     * symbol is added to point to the heap start.
     *
     */

    .ext_flash :
    {
        PROVIDE (_alt_partition_ext_flash_start = ABSOLUTE(.));
        *(.ext_flash)
        . = ALIGN(32 / 8);
        PROVIDE (_alt_partition_ext_flash_end = ABSOLUTE(.));
    } > ext_flash

    .ext_ram :
    {
        PROVIDE (_alt_partition_ext_ram_start = ABSOLUTE(.));        
        *(.ext_ram)
        . = ALIGN(32 / 8);
        PROVIDE (_alt_partition_ext_ram_end = ABSOLUTE(.));
    } > ext_ram

    .onchip_ram_64_kbytes :
    {
        PROVIDE (_alt_partition_onchip_ram_64_kbytes_start = ABSOLUTE(.));
        *(.onchip_ram_64_kbytes)
        . = ALIGN(32 / 8);
        PROVIDE (_alt_partition_onchip_ram_64_kbytes_end = ABSOLUTE(.));
    } > onchip_ram_64_kbytes

    .sdram :
    {
        PROVIDE (_alt_partition_sdram_start = ABSOLUTE(.));
        *(.sdram)
        . = ALIGN(32 / 8);
        PROVIDE (_alt_partition_sdram_end = ABSOLUTE(.));
        _end = ABSOLUTE(.);
        PROVIDE (end = ABSOLUTE(.));
    } > sdram

    /*
     * Stabs debugging sections.
     *
     */

    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
/* provide a pointer for the stack */


