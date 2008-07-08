/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2003-2004 by Ryosuke Takeuchi
 *              Platform Development Center RICOH COMPANY,LTD. JAPAN
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id: sys_config.c,v 1.3 2005/11/24 12:41:23 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール（OAKS16 MINI用）
 */


#include "jsp_kernel.h"
#include <sil.h>
#include "oaks16mini.h"

/*
 *  ターゲットシステム依存 初期化ルーチン
 */

void
sys_initialize(void)
{
	VB * p;
	/* コントロール#2レジスタの初期化 */
	p = (VB*)TADR_SFR_UART0_BASE;
	sil_wrb_mem((VP)(p+TADR_SFR_UC2_OFFSET), 0x00);
	/* UAERT1の設定 */
	p = (VB*)TADR_SFR_UART1_BASE;
  			/* 送受信モードレジスタの初期化 */
	sil_wrb_mem((VP)(p+TADR_SFR_UMR_OFFSET), 0x05);	/* 送受信モ−ドレジスタ 内部クロック,*/
													/* 非同期、8ビット、パリティなし、	*/
													/* スリープなし						*/
			/* 送信制御レジスタの初期化 */
	sil_wrb_mem((VP)(p+TADR_SFR_UC0_OFFSET), 0x10);	/* 送受信制御レジスタ０ クロックf1選択 */
			/* 転送速度レジスタの初期化 */
	sil_wrb_mem((VP)(p+TADR_SFR_UBRG_OFFSET), 64);	/* 転送速度レジスタ(19200bps)		*/
			/* 送受信制御レジスタの初期化 */
	sil_wrb_mem((VP)(p+TADR_SFR_UC1_OFFSET), (TBIT_UiC1_TE | TBIT_UiC1_RE));
													/* 送受信制御レジスタ１ 送受信許可	*/

	/* ポート7の初期化 */
	sil_wrb_mem((VP)TADR_SFR_P7, 0xff);				/* ポート7データ初期化				*/
	sil_wrb_mem((VP)TADR_SFR_PD7, 0xff);			/* ポート7出力設定					*/

	/* ポート8の初期化 */
	sil_wrb_mem((VP)TADR_SFR_PD8, 0x00);			/* ポート8入力設定					*/
	sil_wrb_mem((VP)TADR_SFR_PUR2, 0x01);			/* P80からP83プルアップ				*/
}

/*
 *  ターゲットシステムの終了ルーチン
 */

void
sys_exit(void)
{
  while (1)
  	;
}

void
sys_putc(char c)
{
	if(c == '\n'){
	    oaks16_putc( '\r' );
	}
    oaks16_putc( c );
}

