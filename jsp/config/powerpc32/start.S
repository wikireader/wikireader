/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 * 
 *  @(#) $Id: start.S,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *	カーネル用のスタートアップモジュール（PowerPC用）
 *	
 *	レジスタの割り当て
 *		r1　 ：スタックポインタ
 *		SPRG0：割込み／CPU例外ネストカウンタ
 *		SPRG1：int_table
 *			　　割込み用擬似ベクタテーブルの先頭アドレス
 *		SPRG2：exc_table
 *			　　CPU例外用擬似ベクタテーブルの先頭アドレス
 */

#define	_MACRO_ONLY
#include <t_config.h>

	.text
	.globl start
	.globl start_1
start:
	/*
	 *  プロセッサモードとスタックポインタの初期化
	 *　　リセット時のMSRの初期値そのままでよければ、
	 *　　INIT_MSRは定義しなくてもよい。
	 */
#ifdef INIT_MSR
	LI32(r3, INIT_MSR)		/*  MSRの初期化  */
	mtmsr	r3
#endif	/*  INIT_MSR  */
	
	/*
	 * 非タスクコンテキストのスタックをSTACKTOPに初期化する
	 * STACKTOPは、sys_config.hで定義
	 * コーリング・コンベンションの関係で8バイトずらしておく
	 */
	LI32(sp, STACKTOP - STACK_MARGIN)
	
	/*  割込み／CPU例外ネストカウンタの初期化  */
	li	r5, 1
	mtspr	SPRG0, r5
	
	/*  割込み用擬似ベクタテーブルの先頭アドレス  */
	LI32(r6, int_table)
	mtspr	SPRG1, r6
	
	/*  CPU例外用擬似ベクタテーブルの先頭アドレス  */
	LI32(r7, exc_table)
	mtspr	SPRG2, r7
	
	/*
	 *  hardware_init_hook の呼出し（0 でない場合）
	 *
	 *  ハードウェア依存に必要な初期化処理がある場合は，
	 *  hardware_init_hook という関数を用意すればよい．
	 *  具体的にはROM化の際、RAMを使用可能にするための
	 *  バスコントローラの初期化等を行う。
	 *  sys_support.S 内で hardware_init_hook を定義してい
	 *  る．
	 */
	LI32(r8, hardware_init_hook)
	mtctr	r8
	cmpwi	crf0, r8, 0
	bnectrl

	/*
	 *  bssセクションのクリア
	 */
start_1:
	LI32(r5, __bss_start)
	LI32(r6, _end)
	cmpw	crf0, r5, r6
	beq	start_3
	li	r0, 0
	subi	r5, r5, 4
	subi	r6, r6, 4
start_2:
	stwu	r0, 4(r5)	/*  r5の更新も1命令で実行される  */
	cmpw	crf0, r5, r6
	blt	start_2	

	/*
	 *  dataセクションの初期化（ROM化対応）
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降に
	 *  コピーする．
	 */
start_3:
	LI32(r7, __idata_start)
	LI32(r8, __idata_end)
	cmpw	crf0, r7, r8
	beq	start_5
	LI32(r9, __data_start)
	subi	r7, r7, 4	/*  lwzu命令を使うため-4している  */
	subi	r8, r8, 4
	subi	r9, r9, 4
start_4:
	lwzu	r10, 4(r7)	/*  r7の更新も1命令で実行される  */
	stwu	r10, 4(r9)	/*  r9の更新も1命令で実行される  */
	cmpw	crf0, r7, r8
	blt	start_4

	/*
	 *  software_init_hook を呼出し（0 でない場合）
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
start_5:
	LI32(r4, software_init_hook)
	mtctr	r4
	cmpwi	crf0, r4, 0
	bnectrl

	/*
	 *  カーネルを起動する．
	 */
start_6:
	LI32(r5, kernel_start)
	mtctr	r5
	bctr

/*  end of file  */
