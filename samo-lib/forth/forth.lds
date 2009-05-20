OUTPUT_FORMAT("elf32-c33", "elf32-c33",
              "elf32-c33")
OUTPUT_ARCH(c33)

SECTIONS
{
	.dst_ram  0x84000 : {
/*
		traps.o(.data)
		traps.o(.*)
*/
	}


/*
	.iram 0x0 : {
		suspend.o(.*)
	}
*/

	__dp = 0x10000000 ;

	/**
	 * assign 2K of the RAM for IDMA transfer
	 * must be aligned to a 16 bytes boundry
	 */
	.idma 0x10000000 : {
	   . = . + 2048;
        }

        .rodata : {
           __START_rodata = . ;
           *(.rodata)
        }
        __END_rodata = . ;

        .bss : {
           __START_bss = . ;
           *(.bss)
        }
         __END_bss = . ;

        . = ALIGN(1024);
        .data : {
           __START_data = . ;
           *(.data)
        }

        .user_variables : {
           __START_user_variables = . ;
           *(.user_variables)
        }

        . = ALIGN(1024);
	.text : {
           __START_text = . ;
	   forth.o(*.text)
           *(.text)
        }
        __END_text = . ;

        .forth_dict : {
           __START_forth_dict = . ;
	   forth.o(*.forth_dict)
        }

        .forth_param : {
           __START_forth_text = . ;
	   forth.o(*.forth_param)
        }

        __START_idma = ADDR(.idma);
        __END_idma = __START_idma + SIZEOF(.idma);

        __START_data_lma = LOADADDR( .data );
        __END_data_lma = __START_data_lma + SIZEOF( .data );
}
