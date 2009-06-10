OUTPUT_FORMAT("elf32-c33", "elf32-c33",
              "elf32-c33")
OUTPUT_ARCH(c33)

MEMORY
{
        a0ram  (rwx): ORIGIN = 0x00000000, LENGTH = 8k
        ivram  (rwx): ORIGIN = 0x00080000, LENGTH = 12k
        dstram (rwx): ORIGIN = 0x00084000, LENGTH = 2k
        sdram  (rwx): ORIGIN = 0x10000000, LENGTH = 32M
}

SECTIONS
{
	.dst_ram : {
           __START_VectorTable = . ;
/*
		traps.o(.data)
		traps.o(.*)
*/
	} > dstram


/*
	.iram : {
		suspend.o(.*)
	} > iram
*/

	__dp = 0x10000000 ;

        .idma : {
          . = . + 2048 ;
        } > sdram

	.text : {
           __START_text = . ;
	   forth.o(*.text)
           *(.text)
        } > sdram
        __END_text = . ;

        . = ALIGN(1024);

        .rodata : {
           __START_rodata = . ;
           *(.rodata)
        } > sdram
        __END_rodata = . ;

        .bss : {
           __START_bss = . ;
           *(.bss)
        } > sdram
         __END_bss = . ;

        .data : {
           __START_data = . ;
           *(.data)
        } > sdram
        __END_data = . ;

        . = ALIGN(1024);

        .forth_dict : {
           __START_forth_dict = . ;
	   forth.o(*.forth_dict)
        } > sdram
        __END_forth_dict = . ;


        __START_idma = ADDR(.idma);
        __END_idma = __START_idma + SIZEOF(.idma);

        __START_data_lma = LOADADDR( .data );
        __END_data_lma = __START_data_lma + SIZEOF( .data );
}
