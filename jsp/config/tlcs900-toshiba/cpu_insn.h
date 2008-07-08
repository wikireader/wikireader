/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2006 by Witz Corporation, JAPAN
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
 *  @(#) $Id: cpu_insn.h,v 1.1 2006/04/10 08:19:25 honda Exp $
 */

/*
 *	プロセッサの特殊命令のインライン関数定義（TLCS-900L1用）
 */

#ifndef	_CPU_INSN_H_
#define	_CPU_INSN_H_

/*
 *  ステータスレジスタ（SR）の現在値の読出し
 */
#pragma inline current_sr
UH
current_sr()
{
	Asm("	push	sr");
	Asm("	pop		hl");
	return(__HL);
}

/*
 *  ステータスレジスタ（SR）の現在値の変更
 */
#pragma inline set_sr
void
set_sr(UH sr)
{
	__HL = sr;
	Asm("	push	hl");
	Asm("	pop		sr");
}

/*
 *  NMIを除くすべての割込みを禁止
 */
/*
 *  「ei #7」と「di」は同等の機能であるがei命令は実行後直ちに有効に
 *  なるためei命令を採用する
 */
#pragma inline disint
void
disint()
{
	Asm("	ei	7	");
}

/*
 *  すべての割込みを許可
 */
#pragma inline enaint
void
enaint()
{
	Asm("	ei	0	");
}

/*
 *  レディキューサーチのためのビットマップサーチ関数
 *
 *  ビットマップの下位16ビットを使用し，最下位ビットを最低優先度に対応
 *  させる．
 */
#define	CPU_BITMAP_SEARCH

#pragma inline bitmap_search
UINT
bitmap_search(UINT bitmap)
{
	/*
	 *  このコードは，bitmap（UINT型）が16ビットであることを仮定し
	 *  ている．
	 */
	__HL = bitmap;
	Asm("	bs1f	a, hl");
	return (UINT)__A;
}

#endif /* _CPU_INSN_H_ */
