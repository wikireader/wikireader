OUTPUT_FORMAT("elf32-c33", "elf32-c33",
              "elf32-c33")
OUTPUT_ARCH(c33)

SECTIONS
{
        /* section information */
	. = 0x10008000 ;
	.dst_ram : {
		traps.o(.data)
		traps.o(.*)
		suspend.o(.*)
	}

	. = 0x10000000 ;
	__dp = . ;

        .rodata : {
		
           __START_rodata = . ;
	   __dp = . ;
           *(.rodata)
        }
        __END_rodata = . ;

        .bss : {
           __START_bss = . ;
           *(.bss)
        }
         __END_bss = . ;

        .data : {
           __START_data = . ;
	   . = ALIGN(1024);
           *(.data)
        }
        
	.text : {
           __START_text = . ;
	   mahatma.o(*.text)
           *(.text)
        }
        __END_text = . ;

	.comment : {
	   *(.comment)
	}

        __START_data_lma = LOADADDR( .data );
        __END_data_lma = __START_data_lma + SIZEOF( .data );
}

