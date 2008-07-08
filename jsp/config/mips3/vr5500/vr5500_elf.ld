OUTPUT_FORMAT("elf32-littlemips","elf32-littlemips","elf32-littlemips")
OUTPUT_ARCH(mips)

MEMORY
{
	ROM0(rxai):ORIGIN = 0xbfc00000, LENGTH =      0x700	/* kseg0 */
	ROM1(rxai):ORIGIN = 0x9fc00700, LENGTH =   1M-0x700	/* kseg1 */
/*	ROM1(rxai):ORIGIN = 0xbfc00700, LENGTH =   1M-0x700*/	/* kseg0 */
	RAM (wxai):ORIGIN = 0xa8000000, LENGTH =  64M		/* kseg0 */
		/* SDRAM の最初から取っていないのは、最初の2MB分は
		   ROM のミラー用に確保するため */
}

PROVIDE(_stack_top = 0);
PROVIDE(hardware_init_hook = 0);
PROVIDE(software_init_hook = 0);
PROVIDE(software_term_hook = 0);

SECTIONS
{
	_stack_top = 0xabfffffc;	/* = 0xac000000 - 0x4 (DRAM) ; kseg0 */

	.exce_vec_map :
	{
		*(.exce_vec_map)
	} > ROM0

	.reset 0xbfc00400 :		/* キャッシュ初期化などを含む */
	{
		*(.reset)
	} > ROM0

	.text :
	{
		_stext = . ;
		*(.text)
		_etext  = . ;
	} > ROM1

	.rodata :
	{
		*(.rodata)
	} > ROM1

	__idata_start = .;
	.data : AT ( __idata_start )
	{
		__data_start  = . ;
		*(.data)
	} > RAM

	/* 理由は良く分からないけれども、こう書くのが一般的らしい。 */
	_gp = . + 0x8000;

	/* sdata セクションは chk で引っかかるので、保留。
	   コンパイル時に、-G 0 オプションで避けています。 */

	__isdata_start = .;
	.sdata : AT ( __isdata_start + SIZEOF(.data) )
	{
		*(.lit8) 
		*(.lit4) 
		*(.sdata) 
	} > RAM
	__idata_end = __idata_start + SIZEOF(.data) + SIZEOF(.sdata);

	__bss_start = .	;

	.sbss : 
	{
		_fbss = . ;
		*(.sbss)
		*(.scommon)
	} > RAM

	.bss : 
	{
		*(.bss)
		*(COMMON)
		__bss_end = .;
		_end = . ;
	} > RAM

	PROVIDE (end = .);

	.comment 	0 : { *(.comment) }
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
}

PROVIDE(memcpy = _dummy_memcpy);
