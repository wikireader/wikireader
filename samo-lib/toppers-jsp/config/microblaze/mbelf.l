/*
 *  @(#) $Id: mbelf.l,v 1.4 2004/01/09 10:32:36 honda Exp $
 */

PROVIDE(_hardware_init_hook = 0);
PROVIDE(_software_init_hook = 0);
PROVIDE(software_term_hook = 0);
PROVIDE(__idata_start = 0);
PROVIDE(__idata_end = 0);
PROVIDE(__isdata2_start = 0);
PROVIDE(__isdata2_end = 0);
PROVIDE(__isdata_start = 0);
PROVIDE(__isdata_end = 0);
PROVIDE(__data_start = 0);
PROVIDE(__sdata2_start = 0);
PROVIDE(__sdata_start = 0);

/*
 * Specify the default entry point to the program
 */
ENTRY(_start)

SECTIONS
{
/*
 * Specify that the .text section from all input object files will be placed
 * in LMB memory into the output file section .text Note that mb-gdb expects
 * the executable to have a section called .text
 */
 
    .text : {
     /* Uncomment the following line to add specific files in the opb_text */
     /* region */
     /* *(EXCLUDE_FILE(file1.o).text) */
     /* Comment out the following line to have multiple text sections */
     __text = . ;
     *(.text)
    } 
    _extext = . ;

    . = ALIGN(4);
    .rodata : {
     *(.rodata)
    } 


    . = ALIGN(4);
    .data : {
       *(.data)
    } 

 /*
  * Alignments by 8 to ensure that _SDA2_BASE_ on a word boundary
  */
    . = ALIGN(8);
    _ssrw = .;
    .sdata2 : {
        *(.sdata2)
    } 
    . = ALIGN(8);
    _essrw = .;
    _ssrw_size = _essrw - _ssrw;
    _SDA2_BASE_ = _ssrw + (_ssrw_size / 2 );


 /*
  * Alignments by 8 to ensure that _SDA_BASE_ on a word boundary 
  * Note that .sdata and .sbss must be contiguous
  */
    . = ALIGN(8);
     _ssro = .;
    .sdata : {
        *(.sdata)
    } 

    . = ALIGN(4);
    __bss_start = . ;
    .sbss : {
        *(.sbss)
    } 

    . = ALIGN(8);
    _essro = .;
    _ssro_size = _essro - _ssro;
    _SDA_BASE_ = _ssro + (_ssro_size / 2 );

    /*
     * Define space for the stack and heap 
     * Note that variables _heap must be set to the beginning of this area 
     * and _stack set to the end of this area
     */
   . = ALIGN(4);
   _heap = .;
   .bss : {
    *(.bss)
    *(COMMON)
   } 
   _stack = .;
   . = ALIGN(4);
   _end = .;
    .comment 0 : { *(.comment) }	
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }	
}

