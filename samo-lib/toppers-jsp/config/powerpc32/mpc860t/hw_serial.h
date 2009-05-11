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
 *  @(#) $Id: hw_serial.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *　　シリアルI/Oデバイス（SIO）ドライバ（MPC860T用）
 *　　　　MPC860T内蔵シリアル・マネージメントコントローラSMC1を使用
 *
 *　　　　smc.{c,h}とhw_serial.hの分割の基準
 *　　　　　　・smc.{c,h}：シリアルデバイスに依存する部分のみ記述
 *　　　　　　・hw_serial.h：シリアルデバイス以外に起因する事項は
 *　　　　 　　　　　　　　　hw_serial.hに記述する
 *　　　　　　　　　　　　　　割込みコントローラ依存など
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <s_services.h>
#ifndef _MACRO_ONLY
#include <mpc860_smc.h>
#endif /* _MACRO_ONLY */

/*
 *  SIOの割込みハンドラのベクタ番号（送受信共通）
 */
#define INHNO_SERIAL	INTNO_SMC1

/*
 *　SIUから見たCPM割込みソースの割込みレベルは一まとめになり、
 *　CPM内での割込みレベルはハードウェアにより決定される。
 *　そのため、SMC1固有のIPMは用意していない。
 */

#ifndef _MACRO_ONLY

/*
 *  SIOドライバの初期化ルーチン
 */
#define	sio_initialize	smc_initialize

/*
 *  シリアルI/Oポートのオープン
 */
Inline SIOPCB *
sio_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB	*siopcb;

	/*
	 *  デバイス依存のオープン処理．
	 */
	siopcb = smc_opn_por(siopid, exinf);

    	/*
    	 *  CPM割込みコントローラの設定
    	 *  　CPM割込みコンフィギュレーション・レジスタCICRの
    	 *  　設定はCPUの初期化で行っているとする
    	 *  　（割込みコントローラ依存部）
    	 *  　
    	 *  　CPM内での割込み順位は固定されている。
    	 *  　（SCCを除く）
    	 */
    	mpc860_orw_mem(CIMR, CIMR_SMC1);	/*  SMC1割込みを許可  */

	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
Inline void
sio_cls_por(SIOPCB *siopcb)
{
	/*
	 *  デバイス依存のクローズ処理．
	 */
	smc_cls_por(siopcb);

	if (!smc_openflag()) {
		/*  すべてのポートがクローズされていれば  */
		/*  シリアルデバイスへのクロック供給停止  */
	}

    	/*
    	 *  CPM割込みコントローラの設定
    	 *  　（割込みコントローラ依存部）
    	 */
    	mpc860_andw_mem(CIMR, ~CIMR_SMC1);	/*  SMC1割込みを禁止  */
}

/*
 *  SIOの割込みハンドラ（送受信共通）
 */
#define	sio_handler	smc_isr

/*
 *  シリアルI/Oポートへの文字送信
 */
#define	sio_snd_chr	smc_snd_chr

/*
 *  シリアルI/Oポートからの文字受信
 */
#define	sio_rcv_chr	smc_rcv_chr

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
#define	sio_ena_cbr	smc_ena_cbr

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
#define	sio_dis_cbr	smc_dis_cbr

/*
 *  シリアルI/Oポートからの送信可能コールバック
 *  　　jsp/systask/serial.cにある実体をリネームしている
 *  　　（一種の前方参照）
 */
#define	sio_ierdy_snd	smc_ierdy_snd

/*
 *  シリアルI/Oポートからの受信通知コールバック
 *  　　jsp/systask/serial.cにある実体をリネームしている
 *  　　（一種の前方参照）
 */
#define	sio_ierdy_rcv	smc_ierdy_rcv

#endif /* _MACRO_ONLY */
#endif /* _HW_SERIAL_H_ */
/*  end of file  */
