/* grifo.lds - linker script */

/*
 * Notes:
 *
 * 1. LENGTH() does not work so have to use hard coded numbers
 * 2. ORIGIN() also does not work
 * 3. __START_x, __END_x are byte aligned addresses
 * 4. __SIZE_x is the number of bytes in the block
 */

OUTPUT_FORMAT("elf32-c33")

OUTPUT_ARCH(c33)

MEMORY
{
    /* sdram must be first or linker gives errors */
    ksdram  (rwx): ORIGIN = 0x10000000,        LENGTH = 256k
    sdram   (rwx): ORIGIN = 0x10000000 + 256k, LENGTH = 32M - 256k
    a0ram   (rwx): ORIGIN = 0x00000000,        LENGTH = 8k
    ivram   (rwx): ORIGIN = 0x00080000,        LENGTH = 12k
    dstram  (rwx): ORIGIN = 0x00084000,        LENGTH = 2k
}

SECTIONS
{
    .internal (NOLOAD) : {
         __START_VectorTable = . ;
         . = . + 1k ;       /* 256 entry vector table = 1 kibi bytes */
         __END_VectorTable = . ;
         __START_SuspendCode = . ;
         . = . + 7k ;       /* suspend can use the remainder */
         __END_SuspendCode = . ;
    } > a0ram

    __SIZE_VectorTable = __END_VectorTable - __START_VectorTable ;
    __ENTRIES_VectorTable =  __SIZE_VectorTable / 4 ; /* each entry is one word (4 bytes) */

    .lcd (NOLOAD) : {
        __START_FrameBuffer = . ;
        . = . + 12k /* LENGTH(ivram) */ ;
        __END_FrameBuffer = . ;
    } > ivram

    .descriptor (NOLOAD) : {
        __START_descriptors = . ;
        . = . + 2k /* LENGTH(dstram) */ ;
        __END_descriptors = . ;
    } > dstram

    /* the compiler/linker assumes r15 is preset to this value at the start */
     __dp = 0x10000000 /* ORIGIN(ksdram) */ ;

    .text : {
         __START_text = . ;
         build/main.o(*.text)
         *(.text)
    } > ksdram
    __END_text = . ;

    . = ALIGN(1024);

    .rodata : {
         __START_rodata = . ;
         *(.rodata)
    } > ksdram
    __END_rodata = . ;

    .bss : {
         __START_bss = . ;
         *(.bss)
    } > ksdram
     __END_bss = . ;

    .data : {
         __START_data = . ;
         *(.data)
    } > ksdram
    __END_data = . ;

    . = ALIGN(1024);

    __END_program = . ;


    .user_area (NOLOAD) : {
        __dp_user = . /* ORIGIN(usdram) */ ;
    } > sdram



    /*__MAIN_STACK = ORIGIN(sdram) + LENGTH(sdram) - 4; */
    __MAIN_STACK = 0x10000000 + 32M - 4;
    __MAIN_STACK_LIMIT = __MAIN_STACK - 1M;


    __START_text = ADDR(.text);
    __END_text = __START_text + SIZEOF(.text);

    __START_rodata = ADDR(.rodata);
    __END_rodata = __START_rodata + SIZEOF(.rodata);

    __START_data = ADDR(.data);
    __END_data = __START_data + SIZEOF(.data);

    __START_bss = ADDR(.bss);
    __END_bss = __START_bss + SIZEOF(.bss);

    /* make sure heap is aligned to 1 kibi byte boundary */
    __START_heap = (__END_program + 1023) & (~ 1023);
    __END_heap = __MAIN_STACK_LIMIT & (~ 1023);
    __SIZE_heap = __END_heap - __START_heap;
}
