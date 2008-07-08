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
 *  Copyright (C) 2003-2004 by Katsuhiro Amano
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
 *  @(#) $Id: h8_sil.c,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

/*
 * システムインタフェースレイヤ（追加部分）
 *　　I/Oポートのデータ・ディレクション・レジスタDDRへのアクセス
 *　　H8のDDRは書き込み専用であり、そのままでは所望のビットだけを
 *　　変更することができない。（bset,bclr命令でも回避不可）
 *　　そのため、本実装では、メモリ上にテンポラリを用意して、DDRの
 *　　現在値を保持する方法を採っている。
 *　　
 *　　ポート7は入力専用のため、省略している。
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"
#include <h8_sil.h>

/*  メモリ上のテンポラリ領域  */
static UB ddr_tmp[] = {
	/* 1         2         3         4         5      */
	H8P1DDR0, H8P2DDR0, H8P3DDR0, H8P4DDR0, H8P5DDR0, 
	/* 6         8         9         A         B      */
	H8P6DDR0, H8P8DDR0, H8P9DDR0, H8PADDR0, H8PBDDR0
};

/*  各ポートのアドレス  */
static const VP ddr_adr[] = {
	/*    1            2            3            4            5    */
	(VP)H8P1DDR, (VP)H8P2DDR, (VP)H8P3DDR, (VP)H8P4DDR, (VP)H8P5DDR, 
	/*    6            8            9            A            B    */
	(VP)H8P6DDR, (VP)H8P8DDR, (VP)H8P9DDR, (VP)H8PADDR, (VP)H8PBDDR
};


/*
 *      DDRの読み出し
 */
UB sil_reb_ddr(IO_PORT port)
{
	assert((IO_PORT1 <= port) && (port <= IO_PORTB));
	return ddr_tmp[port];
}

/*
 *      DDRの書き込み
 */
void sil_wrb_ddr(IO_PORT port, UB data)
{
	assert((IO_PORT1 <= port) && (port <= IO_PORTB));
	ddr_tmp[port] = data;
	sil_wrb_mem(ddr_adr[port], (VB)data);
}

/*
 *      DDRのAND演算
 */
void sil_anb_ddr(IO_PORT port, UB data)
{
	UB ddr = sil_reb_ddr(port);
	
	ddr &= data;
	sil_wrb_ddr(port, ddr);
}

/*
 *      DDRのOR演算
 */
void sil_orb_ddr(IO_PORT port, UB data)
{
	UB ddr = sil_reb_ddr(port);

	ddr |= data;
	sil_wrb_ddr(port, ddr);
}

