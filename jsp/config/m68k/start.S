/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: start.S,v 1.6 2003/06/04 01:51:39 hiro Exp $
 */

/*
 *	カーネル用のスタートアップモジュール（M68040用）
 */

#define	_MACRO_ONLY
#include <t_config.h>

	.text
	.globl start
start:
	/*
	 *  プロセッサモードとスタックポインタの初期化
	 */
	cinva %bc			/* すべてのキャッシュを無効化 */
	move.l #0x80008000, %d0		/* 両方のキャッシュを ON */
	movec.l %d0, %cacr

	move.w %sr, %d0
	or.w #0x2700, %d0		/* スーパバイザモード，IPM=7に設定 */
	and.w #0xefff ,%d0		/* 割込みモードに設定 */
	move.w %d0, %sr

	movea.l #STACKTOP, %sp		/* スタックポインタ（SPI）の初期化 */
	movea.l #0, %fp			/* フレームポインタの初期化 */

	/*
	 *  hardware_init_hook の呼出し（0 でない場合）
	 *
	 *  ターゲットハードウェアに依存して必要な初期化処理がある場合
	 *  は，hardware_init_hook という関数を用意する．
	 */
	move.l #hardware_init_hook, %a0
	cmp.l #0, %a0
	jbeq start_1
	jsr (%a0)

	/*
	 *  bssセクションのクリア
	 */
start_1:
	move.l #__bss_start, %a0
	move.l #_end, %d0
	sub.l %a0, %d0
	jbeq start_3
start_2:
	clr.b (%a0)+
	dbra %d0, start_2
	clr.w %d0
	subq.l #1, %d0
	jbcc start_2

	/*
	 *  dataセクションの初期化（ROM化対応）
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降に
	 *  コピーする．
	 */
start_3:
	move.l #__idata_end, %d0
	sub.l #__idata_start, %d0
	jbls start_5
	move.l #__idata_start, %a0
	move.l #__data_start, %a1
start_4:
	move.b (%a0)+, (%a1)+
	dbra %d0, start_4
	clr.w %d0
	subq.l #1, %d0
	jbcc start_4

	/*
	 *  software_init_hook を呼出し（0 でない場合）
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
start_5:
	move.l #software_init_hook, %a0
	cmp.l #0, %a0
	jbeq start_6
	jsr (%a0)

	/*
	 *  カーネルを起動する．
	 */
start_6:
	jmp kernel_start
