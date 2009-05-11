/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2002-2004 by Hokkaido Industrial Research Institute, JAPAN
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
 *  @(#) $Id: start.S,v 1.5 2005/07/06 00:45:07 honda Exp $
 */

/*
 *	カーネル用のスタートアップモジュール（SH1/2用）
 */

#define	_MACRO_ONLY
#include <t_config.h>

/*
 *　　例外ベクタの定義（SH1/2用）
 *　　　ここではリセット時に必要な最低限のベクタしか用意しない
 *　　　cpu_initialize()で例外ベクタの移動（VBRの書き換え）を行う
 */
#ifdef KERNEL_HAS_A_VECTOR_TABLE

/*
 *   以下の例外要因でデフォルトとは異なる例外ベクタを定義
 *   する場合は、sys_config.hで該当するマクロを定義する。
 */
	.section ".vector","ax"
	.align 2
	.long   TO_ASM(RESET_VECTOR)	/*   0:パワーオンリセット  */
	.long   INIT_STACK		/*       SPの初期値        */
	.long   TO_ASM(RESET_VECTOR)	/*   2:マニュアルリセット  */
	.long   INIT_STACK		/*       SPの初期値        */
	.long   TO_ASM(GII_VECTOR)	/*   4:一般不当命令        */
	.long   TO_ASM(RESERVED_VECTOR)	/*   5:システム予約        */
	.long   TO_ASM(SII_VECTOR)	/*   6:スロット不当命令    */
	.long   TO_ASM(RESERVED_VECTOR)	/*   7:システム予約        */
	.long   TO_ASM(RESERVED_VECTOR)	/*   8:システム予約        */
	.long   TO_ASM(CAE_VECTOR)	/*   9:CPUアドレスエラー   */
	.long   TO_ASM(DAE_VECTOR)	/*  10:DMAアドレスエラー   */
	.long   TO_ASM(NMI_VECTOR)	/*  11:MNI                 */
#endif /* KERNEL_HAS_A_VECTOR_TABLE */

/*
 *   SH1/2用スタートアップモジュール
 */
	.global	_start
	.section .text
	.align 2
_start:
	/*
	 *  SRを初期化する。（割込み禁止）
	 *  IPM=0xe
	 *  stub実行時のみIPM=0xfとなる。
	 */
	mov.l _init_sr,r0
	ldc   r0,sr
	/*
	 * 非タスクコンテストのスタックをSTACKTOPに初期化する
	 * STACKTOPは、user_config.hで定義
	 */
	mov.l  _stack_k, r15

	/*
	 *  _hardware_init_hook を呼び出す。(0でない場合)
	 *
	 *  ハードウェア依存に必要な初期化処理がある場合は，
	 *  hardware_init_hook という関数を用意すればよい．
	 *  具体的にはROM化の際、RAMを使用可能にするための
	 *  バスコントローラの初期化等を行う。
	 *  sys_support.S 内で hardware_init_hook を定義してい
	 *  る．
	 */
start_0:
	/*
	 *  プロセッサ内蔵RAMが利用できる場合はそのアドレスを
	 *  INNER_RAM_ADDRESSマクロとしてuser_config.hまたは
	 *  コンパイラ・オプションで定義する。
	 *　（内蔵RAMの先頭アドレスではなく、スタックの底で
	 *　　ある点に注意）
	 *
	 *  その場合、hardware_init_hook()関数をC言語で
	 *  記述してよい。
	 *  一般にC言語の関数呼び出しはスタックを使用するので
	 *  バスコントローラの初期化前にできない。
	 */
#ifdef INNER_RAM_ADDRESS
	mov.l  _inner_ram_address_k, r15
#endif /* INNER_RAM_ADDRESS */
	mov.l   _hardware_init_k, r0
	tst    r0,r0
	bt     start_1
	jsr    @r0
        nop


	/*
         * bssセクションをクリア
	 */
start_1:
	/*
         * スタックポインタを外部RAMに再設定する。（上記参照）
	 */
#ifdef INNER_RAM_ADDRESS
	mov.l  _stack_k, r15
#endif /* INNER_RAM_ADDRESS */

	mov.l	_bss_start_k,r0
	mov.l	_end_k,r1
	cmp/eq  r0,r1
	bt      start_3
	mov	#0,r2
start_2:
	/*
	 *  r0のインクリメントが冗長に見えるが
	 *  mov.l r4,@r0+命令はない
	 *	ディスティネーションのアドレスはディクリメントのみ
	 */
        mov.l	r2,@r0
	add	#4,r0
 	cmp/hi	r0,r1
	bt	start_2

	/*
	 *  dataセクションを初期化する（ROM化対応）．
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降に
	 *  コピーする．
	 *
	 *　デバッグ環境によってはdataセクションの認識に不具合があり、
	 *　__idata_start番地にダウンロードすべきデータを__data_start
	 *　番地にダウンロードしてしまうものがある。その場合は
	 *　OMIT_DATA_SECTION_COPYマクロを定義してやることでこのコピー
	 *　作業を省略する。
	 */
start_3:
#ifndef OMIT_DATA_SECTION_COPY	/*  dataセクションのコピーを行う場合  */
	mov.l   __idata_start_k,r1
	mov.l   __idata_end_k,r2
	cmp/eq  r1,r2
	bt      start_5
	mov.l   __data_start_k,r0
start_4:
	mov.l   @r1+,r4
	mov.l   r4,@r0
	cmp/hi  r1,r2
	add     #4,r0
	bt      start_4
#endif 	/* OMIT_DATA_SECTION_COPY */

	/*
	 *  software_init_hook を呼び出す（0 でない場合）．
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
start_5:
	mov.l  _software_init_hook_k, r0
	tst    r0,r0
	bt     start_6
	jsr    @r0
	nop


start_6:
	/*
         *  カーネルを起動する
	 */
	! call the mainline
	mov.l	_kernel_start_k,r0
	jsr	@r0
	nop


	.align 2
_intnest_k:
	.long	_intnest
_init_sr:
	.long	MAX_IPM << 4
_stack_k:
	.long   STACKTOP


#ifdef INNER_RAM_ADDRESS
_inner_ram_address_k:
	.long   INNER_RAM_ADDRESS
#endif /* INNER_RAM_ADDRESS */

_bss_start_k:
	.long   __bss_start

#ifdef FILL_STACK_WITH_0  /*  デバッガでスタック領域を観察し易く  */
			  /*  するため、非タスクコンテキストの    */
			  /*  スタック領域も初期化する            */
_end_k:
	.long   STACKTOP
#else			/*  スタック領域を初期化しない場合  */
_end_k:			/*  　bssセクションだけ初期化する  */
	.long  	_end
#endif	/* FILL_STACK_WITH_0  */


#ifndef OMIT_DATA_SECTION_COPY	/*  dataセクションのコピーを行う場合  */
__idata_start_k:
	.long 	__idata_start
__idata_end_k:
	.long   __idata_end
__data_start_k:
	.long  	__data_start
#endif 	/* OMIT_DATA_SECTION_COPY */

_kernel_start_k:
	.long	_kernel_start
_hardware_init_k:
	.long   _hardware_init_hook
_software_init_hook_k:
	.long   _software_init_hook

