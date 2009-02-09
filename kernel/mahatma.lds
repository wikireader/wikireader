OUTPUT_FORMAT("elf32-c33", "elf32-c33",
              "elf32-c33")
OUTPUT_ARCH(c33)

SECTIONS
{
        /* section information */
	.dst_ram  0x84000 : {
		traps.o(.data)
		traps.o(.*)
	}

	.iram 0x0 : {
		suspend.o(.*)
	}

	__dp = 0x10000000 ;
        .rodata 0x10000000 : {
		
           __START_rodata = . ;
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

