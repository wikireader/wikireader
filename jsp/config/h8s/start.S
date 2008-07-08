/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
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

#define _MACRO_ONLY

#include <t_config.h>	/* CCR_I, MAX_IPM */

/*
 *  H8S用スタートアップモジュール
 *  カーネルはアプリケーションとリンクして使用する
 *  だけなのでJSPカーネルのみ使用する。
 */
	.h8300s

	.text
	.align 2

	.global	start
	.global bss_clear	/* hardware_init_hook からの戻りアドレス */

start:

	/*
	 *  タスク独立のスタックをSTACKTOPに初期化する
	 *  （STACKTOPは、sys_config.hで定義）
	 *  ・ハードウェアマニュアル 「4.2.4 リセット直後の割込み」(P90)より
	 *    プログラムの先頭命令は SP の初期化命令でなければならない。
	 */
	mov.l	#STACKTOP, sp

	/*
	 *  CCR の初期化
	 *  ・割込み制御モード0 における割込み禁止 (I = 1 ; CCR)
	 *    ( _hardware_init_hook 内で割込み制御モードを変更するまでは、
	 *     割込み制御モードは0である。)
	 */
	ldc.b	#CCR_I, ccr

	/*
	 *  EXR の初期化
	 *  ・割込み制御モード2 における割込み禁止 (I2, I1, I0 = 1 ; CCR)
	 */
	ldc.b	#MAX_IPM, exr

	/*
	 *  hardware_init_hook を呼び出す。(0でない場合)
	 *  ・ターゲットハードウェアに依存する初期化処理がある場合は，
	 *    hardware_init_hook という関数を用意する．
	 *      例）ROM化の際、RAMを使用可能にするためのバスコントローラの初期化
	 *    今回は、sys_support.S 内で hardware_init_hook を定義している．
	 */
hardware_init:
	mov.l	@_hardware_init_hook_k, er0
	or.l	er0, er0
	beq	bss_clear		/* er0 = 0 なら bss_clear へ */
	jmp	@er0

	/*
	 * bssセクションをクリア
	 */
bss_clear:
	mov.l	#__bss_start, er0
	mov.l	#_end, er1
	sub.l	er2, er2
loop_bss_clear:
	mov.l	er2, @er0
	add.l	#4, er0
	cmp.l	er1, er0
	blo	loop_bss_clear		/* er1 > er0 までジャンプ */

	/*
	 *  dataセクションを初期化する（ROM化対応）
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降にコピーする
	 */
data_sec_copy:
	mov.l	#__idata_start, er0
	mov.l	#__idata_end, er1
	mov.l	#__data_start, er2
loop_data_sec_copy:
	mov.l	@er0+, er4		/* *er2++ = *er0 ++; */
	mov.l	er4, @er2
	add.l	#4, er2
	cmp.l	er1, er0
	blo	loop_data_sec_copy	/* er1 > er0 までジャンプ */

	/*
	 *  software_init_hook を呼び出す（0 でない場合）
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処理がある
	 *  場合は，software_init_hook という関数を用意する．
	 */
software_init:
	mov.l	@_software_init_hook_k, er0
	or.l	er0, er0
	beq	start_kernel
	jsr	@er0

	/*
	 *  カーネルを起動する
	 */
start_kernel:
	jmp	@_kernel_start

	.align 4
_hardware_init_hook_k:
	.long   _hardware_init_hook
_software_init_hook_k:
	.long   _software_init_hook
