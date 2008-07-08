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
 *  @(#) $Id: user_config.h,v 1.7 2007/03/23 07:58:33 honda Exp $
 */

#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/*
 *  ユーザー・コンフィギュレーション情報の定義
 */

/*
 *  chg_ipmをサポートするかどうかの定義
 *　　アセンブラ用のuser_config_asm.incにも同様の定義が必要なので注意
 */
#define SUPPORT_CHG_IPM

/*
 *  vxget_timをサポートするかどうかの定義
 */
#define SUPPORT_VXGET_TIM

/*
 *  クロック周波数 [Hz]
 *　　　user_config_asm.incにも同様の定義が必要なので注意
 */
#define CPU_CLOCK               24576000ul


/*
 *  シリアルポートの設定
 */

/*
 *  サポートするシリアルデバイスの数（最大 2）
 *　　user_config_asm.incにも同様の定義が必要なので注意
 */
#define TNUM_PORT               1u

/*
 *  ポート番号
 *
 *  　本来はhw_serial.hで定義すべきだが、サンプルプログラムから
 *  　参照するため、ここで定義する。
 */
#define SCI_PORTID1				1		/*  変更禁止  */
#define SCI_PORTID2				2		/*  同上  */

/*
 *  ポート番号の割り当て
 *
 *  　E10T-USBでデバッグを行う際には、SCI1が占有されるため、
 *  　ユーザープログラムはSCI0のみを使用する。
 *
 *　　ポート数１のとき
 *　　　　ボート１(SCI0)：システムポート（固定）
 *　　ポート数２のとき
 *　　　　ボート１(SCI0)：システムポート／ユーザーポート選択可能
 *　　　　ボート２(SCI1)：同上
 */
#if TNUM_PORT == 1u

#define SYSTEM_PORTID           SCI_PORTID1		/*  固定  */

#elif TNUM_PORT == 2u    /* of #if TNUM_PORT == 1u */

#define SYSTEM_PORTID           SCI_PORTID1		/*  交換可能  */
#define USER_PORTID             SCI_PORTID2

#else   /* of #if TNUM_PORT == 1u || TNUM_PORT == 2u */

#error TNUM_PORT <= 2u

#endif  /* of #if TNUM_PORT == 1u */

/*
 *  低レベル出力に用いるシリアルポートのポート番号
 */
#define SCI_LOW_PORTID			SYSTEM_PORTID

/*  ボーレートとプライオリティ・レベル  */
#define SCI_PORT1_BAUD_RATE     38400ul		/* ボーレート [bps]     */
#define SCI_PORT1_IPM           IPM_LEVEL0	/* プライオリティレベル  */

#if TNUM_PORT == 2u

#define SCI_PORT2_BAUD_RATE     38400ul		/* ボーレート [bps]     */
#define SCI_PORT2_IPM           IPM_LEVEL0	/* プライオリティレベル  */

#endif  /* of #if TNUM_PORT == 2u */


/*
 * エラー割り込みを、入力割り込みと別に操作する場合はコメントを外す。
 *　　user_config_asm.incにも同様の定義が必要なので注意
 */
#if 0
#define H8_CFG_SCI_ERR_HANDLER
#endif


/*
 *  タイマの設定
 */

/*
 *  タイムティックの定義
 */

#define TIC_NUME        1u               /* タイムティックの周期の分子 */
#define TIC_DENO        1u               /* タイムティックの周期の分母 */

/*  タイマに供給されるクロック周波数 [kHz]  */
#define TIMER_CLOCK     ((CPU_CLOCK)/8000ul)     /* 8分周 */

/*  プライオリティレベル  */
#define SYSTEM_TIMER_IPM                IPM_LEVEL1


#endif /* _USER_CONFIG_H_ */
