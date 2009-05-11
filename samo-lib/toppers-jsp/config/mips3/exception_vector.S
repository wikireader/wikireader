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

#include "jsp_kernel.h"

	.section ".exce_vec_map","xa"
		/* exce_vec_map セクションの記述は、1kbyte内に収める
		   こと。(0xbfc00000-0xbfc00400 の範囲内)
		   これは、0xbfc00400 からの領域に、init セクション
		   ならびに text セクションが記述されるため。*/

	.set noreorder

	.align 2

/*
 *  例外ベクタアドレスでの分岐処理 (BEV=1を想定)
 */

/*
 *  コールドリセット、ソフトリセット、NMI(Non Maskable Interrupt)
 *     ステータスレジスタのBEVビットに
 *         依存しない：0xbfc00000番地  (kseg1領域；キャッシュなし)
 */

	.global start	/* ここがエントリーポイント */

start:
reset_and_NMI:
	mfc0	k0, Status
	li	k1, SR_SR
	and	k1, k0, k1
	bne	k1, zero, NMI_exception
	nop

	/* コールドリセット時 */
	la	k1, reset
	jr	k1
	nop

	/* ソフトリセット、NMI 時 */
NMI_exception:
	la	k1, reset
	jr	k1
	nop

/*
 *  TLB不一致例外（JSPカーネルではTLB未使用）
 *      ステータスレジスタのBEVビットが
 *          1のとき：0xbfc00200番地  (kseg1領域；キャッシュなし)
 *          0のとき：0x80000000番地  (kseg0領域；キャッシュあり)
 */
	.org 0x200

TLB_miscompare_exception:
	move	a0, sp
	mfc0	a1, Status
	mfc0	a2, Cause
	la	k1, cpu_experr
	jr	k1
	nop

/*
 *  XTLB不一致例外（JSPカーネルではTLB未使用）
 *      ステータスレジスタのBEVビットが
 *          1のとき：0xbfc00280番地  (kseg1領域；キャッシュなし)
 *          0のとき：0x80000080番地  (kseg0領域；キャッシュあり)
 */
	.org 0x280

XTLB_miscompare_exception:
	move	a0, sp
	mfc0	a1, Status
	mfc0	a2, Cause
	la	k1, cpu_experr
	jr	k1
	nop

/*
 *  キャッシュエラー例外
 *      ステータスレジスタのBEVビットが
 *          1のとき：0xbfc00300番地  (kseg1領域；キャッシュなし)
 *          0のとき：0xa0000100番地  (kseg0領域；キャッシュあり)
 */
	.org 0x300

cashe_error_exception:
	move	a0, sp
	mfc0	a1, Status
	mfc0	a2, Cause
	la	k1, cpu_experr
	jr	k1
	nop

/*
 *  その他例外：例外/割り込み出入口処理ルーチン
 *  (MIPSアーキテクチャは割込みベクタテーブルを持たず、割込みも一般例外の
 *   一種として分岐する)
 *      ステータスレジスタのBEVビットが
 *          1のとき：0xbfc00380番地  (kseg1領域；キャッシュなし)
 *          0のとき：0x80000180番地  (kseg0領域；キャッシュあり)
 */

	.org 0x200 + 0x180	/* TLB不一致例外の0x180番地後ろに配置 */

other_exception:
	la	k1, general_exception
	jr	k1
	nop
