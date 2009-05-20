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
	.iram : {
		suspend.o(.*)
	} > a0ram

        /*__PANIC_STACK = ORIGIN(a0ram) + LENGTH(a0ram) - 4; */
        __PANIC_STACK = 0x00000000 + 8k - 4;

        /*__MAIN_STACK = ORIGIN(sdram) + LENGTH(sdram) - 4; */
        __MAIN_STACK = 0x10000000 + 32M - 4;
        __MAIN_STACK_LIMIT = __MAIN_STACK - 1M;
	.dst_ram : {
		traps.o(.data)
		/* traps.o(.*) */
	} > dstram

	/*__dp = ORIGIN(sdram);*/
	__dp = 0x10000000;

	/**
	 * assign 2K of the RAM for IDMA transfer
	 * must be aligned to a 16 bytes boundry
	 */
	.idma : {
	   . = . + 2048;
        } > sdram

	.text : {
	   mahatma.o(*.text)
           *(.text)
        } > sdram

        .rodata : {
           *(.rodata)
        } > sdram

        .data : {
           *(.data)
        } > sdram

        .bss : {
           *(.bss)
        } > sdram

       .comment : {
          mahatma.o(.comment)
       } > sdram

        __START_idma = ADDR(.idma);
        __END_idma = __START_idma + SIZEOF(.idma);

        __START_text = ADDR(.text);
        __END_text = __START_text + SIZEOF(.text);

        __START_rodata = ADDR(.rodata);
        __END_rodata = __START_rodata + SIZEOF(.rodata);

        __START_data = ADDR(.data);
        __END_data = __START_data + SIZEOF(.data);

        __START_bss = ADDR(.bss);
        __END_bss = __START_bss + SIZEOF(.bss);

        __START_heap = (__END_bss + 1023) & (~ 1023);
        __END_heap = __MAIN_STACK_LIMIT & (~ 1023);
        __SIZE_heap = __END_heap - __START_heap;

	__SIZE_kernel = ADDR(.comment) - ADDR(.text);

}
