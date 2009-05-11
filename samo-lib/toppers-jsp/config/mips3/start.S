/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2000-2003 by Industrial Technology Institute,
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

#include "jsp_kernel.h"		/* INIT_SR */

/*
 *   MIPS3用スタートアップモジュール
 *     リセットベクタ  0xbfc00000番地に配置
 *   
 *   カーネルはアプリケーションとリンクして使用するだけなので
 *   JSPカーネルのみ使用する。
 */

	.section ".reset","xa"

	.set noreorder

	.align 2
	.global	reset

reset:
	/*
	 *  CPUレジスタの初期化
	 */

	/* HI, LO (乗除算レジスタ) */
	mthi	zero
	mtlo	zero

	/* sp (スタックポインタ；非タスクコンテキスト用)
	     _stack_top は、システム依存部(リンカスクリプト推奨)定義 */
	la	sp, _stack_top

	/* gp (グローバルポインタ)
	     _gp は、システム依存部(リンカスクリプト推奨)定義 */
	la	gp, _gp

	/* k0 (割込み/CPU例外ネストカウンタ用) */
	li	k0, 0x1		/* タスク起動までは、非タスクコンテキストでの
				   実行とみなす。 */
	/* k1は、既に汎用として利用しているので初期化しない。 */

	/*
	 *  CP0レジスタの初期化
	 */

	/* SR (ステータスレジスタ)
	     (ここでコアの割込みマスクも設定する。これは、実質的な割込み制御は、
	     外部割込みコントローラで行うため。)
	     INIT_SR は、sys_config.h で定義 */
	li	t0, INIT_SR
	mtc0	t0, Status

	/* Config (コンフィグレジスタ)
	     INIT_CONFIG_MASK は、sys_config.h で定義 */
	mfc0	t0, Config
	li	t1, INIT_CONFIG_MASK
	or	t0, t0, t1
	mtc0	t0, Config

	/* WatchLo (ウォッチLoレジスタ) */
	mtc0	zero, WatchLo

	/* Compare (比較レジスタ) */
	li	t0, 0xffffffff
	mtc0	t0, Compare

	/*
	 *  hardware_init_hook を呼び出す。(0でない場合)
	 *
	 *  ・ターゲットハードウェアに依存する初期化処理がある場合は，
	 *    hardware_init_hook という関数を用意する．
	 *      例）ROM化の際、RAMを使用可能にするためのバスコントローラの初期化
	 *    今回は、sys_support.S 内で hardware_init_hook を定義している．
	 */
hardware_init:
	la	t0, hardware_init_hook
	beq	t0, zero, bss_clear
	nop
	jalr	ra, t0
	nop

	/*
         * bssセクションをクリア
	 */
bss_clear:
	la	t1, __bss_start
	la	t2, __bss_end
	beq	t1, t2, data_sec_copy
	nop
        sw	zero, (t1)
loop_bss_clear:
        addi	t1, t1, 4
        sub	t3, t2, t1
        bgez 	t3, loop_bss_clear
        sw	zero, (t1)

	/*
	 *  dataセクションを初期化する (ROM化対応)
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降にコピーする
	 */
data_sec_copy:
	la	t1, __idata_start
	la	t2, __idata_end
	beq	t1, t2, software_init
	nop
	la	t3, __data_start
loop_data_sec_copy:
	lw	t5, (t1)
        sw	t5, (t3)
        addi	t1, t1, 4
        addi	t3, t3, 4
        sub	t4, t2, t1
        bgtz 	t4, loop_data_sec_copy
	nop

	/*
	 *  software_init_hook を呼び出す（0 でない場合）
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処理がある
	 *  場合は，software_init_hook という関数を用意する．
	 */
software_init:
	la	t0, software_init_hook
	beq	t0, zero, start_kernel
	nop
	jalr	ra, t0
	nop

	/*
         *  カーネルを起動する
	 */
start_kernel:
	la	t0, kernel_start
	j	t0
	nop
