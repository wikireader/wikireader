/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 */

OUTPUT_FORMAT("coff-h8300")
OUTPUT_ARCH(h8300h)

ENTRY("_start")

MEMORY {
	/* for mode 5: 0x00000 -> 0xfffff */
        vectors		: o = 0x00000, l = 0x00100

	/* use 128k - 100 as the main ROM size.  */
	rom		: o = 0x00100, l = 0x1ff00

	/* use 384k as the main RAM size.  */
	ram		: o = 0x20000, l = 0x60000

	}

PROVIDE(_hardware_init_hook = 0);
PROVIDE(_software_init_hook = 0);
PROVIDE(_software_term_hook = 0);
PROVIDE(_memcpy = _local_memcpy);
SECTIONS {

	.vectors : {
		*(.vectors)
	} >vectors

	.text :
	{
		__text = . ;
		*(.text)
	} >rom
	_etext = .	;
	PROVIDE (etext = .)	;

	.rodata : {
		*(.rodata)
	} >rom

	. = ALIGN(4);
	__idata_start = . ;
	.data   :  AT(__idata_start)
	{
		__data_start = . ;
		*(.data)
	} >ram
	__idata_end = __idata_start + SIZEOF(.data);
	_edata  =  . ;
	PROVIDE (edata = .);

	. = ALIGN(4);
	__bss_start = .	;
	.bss       :
	{
	  *(.bss)
	  *(COMMON)
	} >ram
	_end = .		;
	PROVIDE (end = .)	;

	.comment 0 : { *(.comment) }
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
}
