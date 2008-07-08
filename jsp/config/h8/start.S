/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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
 * 
 *  @(#) $Id: start.S,v 1.17 2007/03/23 07:22:15 honda Exp $
 */
#define _MACRO_ONLY
#include <t_config.h>
	
/*
 *   H8用スタートアップモジュール
 *   　カーネルはアプリケーションとリンクして使用するので
 *   　このスタートアップモジュールはJSPカーネルでのみ使用する。	
 */
	.h8300h

	.text
	.align 2

	.global	_start
_start:

	/*
	 * 非タスクコンテキスト用スタックをSTACKTOPに初期化する
	 * 　（STACKTOPは、sys_config.hで定義）
	 * _hardware_init_hookを呼び出すにはSTACKTOPが指すRAM
	 * 領域がアクセス可能になっている必要がある。（下記参照）
	 */
	mov.l	#STACKTOP, sp
	
	/*  割込み全禁止  */
	orc.b  #H8INT_DIS_ALL, ccr

	/*
	 *  _hardware_init_hook を呼び出す。(0でない場合)
	 *
	 *  ハードウェア依存に必要な初期化処理がある場合は，
	 *  hardware_init_hook という関数を用意すればよい．
	 *  具体的にはROM化の際、RAMを使用可能にするための
	 *  バスコントローラの初期化等を行う。
	 *  sys_support.S 内で hardware_init_hook を定義してい
	 *  る．
	 *  
	 *  hardware_init_hookではバス・ステートコントローラの
	 *  初期化を行うので、そのルーチンを呼び出すのにRAMを
	 *  スタック領域に使用するのはおかしいが、
         *　ここのサブルーチン呼び出しでは、
         *　　・初期状態のままアクセスできる内蔵RAM　または
         *　　・モニタによりアクセスできる状態にある外部RAM
         *　をスタック領域に使用している。
	 */
start_0:
	mov.l  	#_hardware_init_hook, er0
	beq	start_1
	jsr	@er0
	
	/*
         * bssセクションをクリア
	 */
start_1:		
	mov.l	#__bss_start, er0
	mov.l	#_end, er1
	sub.l	er2, er2
start_2:
        mov.l	er2, @er0
	add.l	#4, er0
 	cmp.l	er1, er0
 	blo	start_2		/* er1 > er0 */

	/*
	 *  dataセクションを初期化する（ROM化対応）．
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降に
	 *  コピーする．
	 *  
	 *  eepmov.w命令を使う方が効率的だが、最大転送バイト数が
	 *  65,535バイトに制限されるため、使用していない。
	 */
	mov.l	#__idata_start, er0
	mov.l	#__idata_end, er1
	mov.l	#__data_start, er2
start_4:
	mov.l	@er0+, er4	/* *er2++ = *er0 ++; */
	mov.l	er4, @er2
	add.l	#4, er2
	cmp.l	er1, er0
	blo	start_4		/* er1 > er0 */

	/*
	 *  software_init_hook を呼び出す（0 でない場合）．
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
	mov.l	#_software_init_hook, er0
	beq	start_6		
	jsr	@er0
		
	/*	
         *  カーネルを起動する
	 */
start_6:
	jmp	@_kernel_start

