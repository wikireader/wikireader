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

/*
 *	シリアルI/Oデバイス（SIO）ドライバ（VR4131内蔵DSIU用）
 */

#ifndef _HW_SERIAL_H_
#define _HW_SERIAL_H_

#include <vr4131_dsiu.h>		/* siopcb, vr4131_dsiu_openflag */

/*
 *  SIOの割込みハンドラのベクタ番号
 */
#define INHNO_SIO		INTNO_DSIU

/*
 *  SIOドライバの初期化ルーチン
 */
#define	sio_initialize		vr4131_dsiu_initialize

/*
 *  カーネル起動時用の初期化 (sys_putcで利用)
 */
#define sio_init		vr4131_dsiu_init

#ifndef _MACRO_ONLY

/*
 *  シリアルI/Oポートのオープン
 */
Inline SIOPCB *
sio_opn_por(ID siopid, VP_INT exinf)
{
	SIOPCB	*siopcb;
	BOOL	openflag;

	/*
	 *  オープンしたポートがあるかを openflag に読んでおく．
	 */
	openflag = vr4131_dsiu_openflag();

	/*
	 * DSIUへのクロック供給開始
	 */
	vr4131_orh( (VP) CMUCLKMSK, (MSKDSIU | MSKSSIU | MSKSIU) );

	/*
	 *  デバイス依存のオープン処理．
	 */
	siopcb = vr4131_dsiu_opn_por(siopid, exinf);

	/*
	 *  シリアルI/O割込みの割込みレベルを設定し，マスクを解除する．
	 */
	if (!openflag) {
		/* 割込みレベル設定 */
		all_set_ilv( (UINT) INTNO_DSIU, &((IPM) IPM_DSIU) );

		/* マスク解除処理(レベル１) */
		vr4131_orh( (VP) MSYSINT2REG, DSIUINTR );

		/* マスク解除処理(レベル２) */
		vr4131_orh( (VP) MDSIUINTREG, INTDSIU );
	}

	return(siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
Inline void
sio_cls_por(SIOPCB *siopcb)
{
	/*
	 *  シリアルI/O割込みをマスクする．
	 */
	if (!vr4131_dsiu_openflag) {
		/* マスク設定処理(レベル１) */
		vr4131_andh( (VP) MSYSINT2REG, ~DSIUINTR );

		/* マスク設定処理(レベル２) */
		vr4131_andh( (VP) MDSIUINTREG, ~INTDSIU );
	}

	/*
	 *  デバイス依存のクローズ処理．
	 */
	vr4131_dsiu_cls_por(siopcb);

	/*
	 *  DSIUへのクロック供給停止
	 */
	vr4131_andh( (VP) CMUCLKMSK, ~(MSKDSIU | MSKSSIU | MSKSIU) );

}

#endif /* _MACRO_ONLY */

/*
 *  SIOの割込みハンドラ
 */
#define	sio_handler		vr4131_dsiu_isr

/*
 *  シリアルI/Oポートへの文字送信
 */
#define	sio_snd_chr		vr4131_dsiu_snd_chr

/*
 *  シリアルI/Oポートへの文字送信（ポーリング）
 */
#define	sio_snd_chr_pol		vr4131_dsiu_putchar_pol

/*
 *  シリアルI/Oポートからの文字受信
 */
#define	sio_rcv_chr		vr4131_dsiu_rcv_chr

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
#define	sio_ena_cbr		vr4131_dsiu_ena_cbr

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
#define	sio_dis_cbr		vr4131_dsiu_dis_cbr

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
#define	sio_ierdy_snd		vr4131_dsiu_ierdy_snd

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
#define	sio_ierdy_rcv		vr4131_dsiu_ierdy_rcv

#endif /* _HW_SERIAL_H_ */
