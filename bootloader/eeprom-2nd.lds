OUTPUT_FORMAT("elf32-c33", "elf32-c33", "elf32-c33")
OUTPUT_ARCH(c33)
SEARCH_DIR(.);
SECTIONS
{

        /* data pointer symbol By GWB33 */
        __dp = 0x1500;

        /* section infomation By GWB33 */
        . = 0x1500;

        .data : AT( __END_rodata )
          {
           __START_data = . ;
           *(.data) ;
           __END_data = . ;
          }
          __END_data_LMA = LOADADDR( .data );

        .text 0x200 :
          {
           __START_text = . ;
           *(.text) ;
           __END_text = . ;
          }

        .rodata 0x1d00 :
          {
           __START_rodata = . ;
           *(.rodata) ;
           __END_rodata = . ;
          }

	.bss : AT( __END_rodata )
          {
           __START_bss = . ;
           *(.bss) ;
           __END_bss = . ;
          }
}

