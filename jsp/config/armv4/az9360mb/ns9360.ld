/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005-2007 by Y.D.K.Co.,LTD Technologies company
 *
 *  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation 
 *  によって公表されている GNU General Public License の Version 2 に記
 *  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
 *  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
 *  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
 *  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: ns9360.ld,v 1.1 2007/05/11 06:06:35 honda Exp $
 */
/*
 * OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm","elf32-littlearm") 
*/
OUTPUT_FORMAT("elf32-bigarm")
OUTPUT_ARCH(arm)
	
PROVIDE(hardware_init_hook = 1);
PROVIDE(software_init_hook = 0);
PROVIDE(software_term_hook = 0);

SECTIONS
{
    . = 0x00 ;    
    .vector :
    {
    *(.vector)
    }
    
    .text :	
    {
        __text = . ; 	
        *(.text)
    }
    _etext = .	;
    PROVIDE (etext = .)	;
    .rodata : { *(.rodata) }
    . = ALIGN(4);
    .ctors :
    {
        PROVIDE(__CTOR_BEGIN__ = .);
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        PROVIDE(__CTOR_END__ = .);
    }
    . = ALIGN(4);
    .dtors :
    {
        PROVIDE(__DTOR_BEGIN__ = .);
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        PROVIDE(__DTOR_END__ = .);
       _endtext = .;
    }
    __idata_start = . ;
  .data :AT (_endtext)
  {
    _etext = _endtext;
    __data_start = .;
    PROVIDE (data_start = .);
    *(.data)
    *(.gnu.linkonce.d*)
    SORT(CONSTRUCTORS)
    *(.data1)
        *(.eh_frame)
        *(.jcr)
        *(.gcc_except_table)
    . = ALIGN(0x10);
  }
    __idata_end = __idata_start + SIZEOF(.data);

    .bss  : AT (_endtext  + SIZEOF(.data))
    {
      __bss_start = . ;
      *(.bss)
      *(COMMON)
      *(.stack)
      . += 12K;
      _stack_end = . ;
      PROVIDE (stack_end  = .)	;
    }

    _end = .		;
    __bss_end = .;
    PROVIDE (end = .)	;
 
   .ttb    : AT (__idata_end + SIZEOF(.bss))
  {
   . = ALIGN(0x4000);
   ttb_start = . ;
   *(.ttb)
   . += 32K;
   ttb_end = . ;
  }

    .comment 0 : { *(.comment) }	
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }	
  .debug_info 0 : { *(.debug_info) }
  .debug_abbrev 0 : { *(.debug_abbrev) }
  .debug_line 0 : { *(.debug_line) }
  .debug_frame 0 : { *(.debug_frame) }
  .debug_str 0 : { *(.debug_str) }
  .debug_loc 0 : { *(.debug_loc) }
  .debug_macinfo 0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames 0 : { *(.debug_varnames) }
  /* These must appear regardless of  .  */
}	

	
		
	