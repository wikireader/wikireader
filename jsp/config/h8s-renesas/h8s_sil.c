/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
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
 *  @(#) $Id: h8s_sil.c,v 1.2 2007/03/23 07:59:26 honda Exp $
 */

/*
 * システムインタフェースレイヤ（追加部分）
 *　　I/Oポートのデータ・ディレクション・レジスタDDRへのアクセス
 *　　H8SのDDRは書き込み専用であり、そのままでは所望のビットだけを
 *　　変更することができない。（bset,bclr命令でも回避不可）
 *　　そのため、本実装では、メモリ上にテンポラリを用意して、DDRの
 *　　現在値を保持する方法を採っている。
 *　　
 *　　ポート４にDDRはないため、欠番にしている。
 */

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

/*  メモリ上のテンポラリ領域  */
static UB ddr_tmp[] = {
        P1DDR0, P2DDR0, P3DDR0, P5DDR0, 	/*  ポート４は欠番  */
        P6DDR0, P7DDR0, P8DDR0, P9DDR0, 
        PADDR0, PBDDR0, PCDDR0, PDDDR0, PEDDR0, 
        PFDDR0, PGDDR0
};

/*  
 *　各ポートのアドレス  
 *　　　アドレスの下位16ビットを保持する。
 */
static const UH ddr_adr[] = {
        P1DDR, P2DDR, P3DDR, P5DDR,  		/*  ポート４は欠番  */
        P6DDR, P7DDR, P8DDR, P9DDR, 
        PADDR, PBDDR, PCDDR, PDDDR, PEDDR,
        PFDDR, PGDDR
};


/*
 *      DDRの読み出し
 */
UB sil_reb_ddr(IO_PORT_ID port)
{
	assert(port <= TNUM_IO_PORT);
	return ddr_tmp[port];
}

/*
 *      DDRの書き込み
 */
void sil_wrb_ddr(IO_PORT_ID port, UB data)
{
	assert(port <= TNUM_IO_PORT);
	ddr_tmp[port] = data;
	h8s_wrb_reg(ddr_adr[port], (VB)data);
}

/*
 *      DDRのAND演算
 */
void sil_anb_ddr(IO_PORT_ID port, UB data)
{
	UB ddr = sil_reb_ddr(port);
        
	ddr &= data;
	sil_wrb_ddr(port, ddr);
}

/*
 *      DDRのOR演算
 */
void sil_orb_ddr(IO_PORT_ID port, UB data)
{
	UB ddr = sil_reb_ddr(port);

	ddr |= data;
	sil_wrb_ddr(port, ddr);
}

