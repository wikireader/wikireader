OUTPUT_FORMAT("elf32-c33", "elf32-c33", "elf32-c33")
OUTPUT_ARCH(c33)
SEARCH_DIR(.);

MEMORY
{
        a0ram  (rwx): ORIGIN = 0x00000000, LENGTH = 8k
        ivram  (rwx): ORIGIN = 0x00080000, LENGTH = 12k
        dstram (rwx): ORIGIN = 0x00084000, LENGTH = 2k
        sdram  (rwx): ORIGIN = 0x10000000, LENGTH = 32M
}

SECTIONS
{
        /* the SPI_exchange and FLASH_read are built into the master boot record
         * to save a little space
         */
        mbr_FLASH_read = 0x2;
        mbr_SPI_exchange = 0x4;

        __dp = 0;

        .text 0x200 :
        {
                *(.text) ;
        } > a0ram

        __START_text = LOADADDR(.text);
        __SIZE_text = SIZEOF(.text);

        .rodata :
        {
                *(.rodata) ;
        } > a0ram

        __START_rodata =  LOADADDR(.rodata);
        __SIZE_rodata =  SIZEOF(.rodata);

        .data :
        {
                *(.data) ;
        } > a0ram

        __START_data =  LOADADDR(.data);
        __SIZE_data =  SIZEOF(.data);

	.bss :
        {
                *(.bss) ;
        } > a0ram

        __START_bss = LOADADDR(.bss);
        __SIZE_bss = SIZEOF(.bss);

	.idma : {
	   . = . + 2048;
        } > dstram

        __START_idma = LOADADDR(.bss);
        __SIZE_idma = SIZEOF(.bss);
}
