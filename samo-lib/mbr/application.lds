OUTPUT_FORMAT("elf32-c33", "elf32-c33", "elf32-c33")
OUTPUT_ARCH(c33)
SEARCH_DIR(.);

MEMORY
{
        iram (rwx): ORIGIN = 0, LENGTH = 8k
}

SECTIONS
{
        /* the eeprom_load is built into the master boot record */
        eeprom_load = 0x2;

        __dp = 0;

        .text 0x200 :
        {
                *(.text) ;
        } > iram

        __START_text = LOADADDR(.text);
        __SIZE_text = SIZEOF(.text);

        .rodata :
        {
                *(.rodata) ;
        } > iram

        __START_rodata =  LOADADDR(.rodata);
        __SIZE_rodata =  SIZEOF(.rodata);

        .data :
        {
                *(.data) ;
        } > iram

        __START_data =  LOADADDR(.data);
        __SIZE_data =  SIZEOF(.data);

	.bss :
        {
                *(.bss) ;
        } > iram

        __START_bss = LOADADDR(.bss);
        __SIZE_bss = SIZEOF(.bss);
}
