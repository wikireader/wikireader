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
 *  @(#) $Id: user_config.h,v 1.2 2007/03/23 07:59:26 honda Exp $
 */

#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/*
 *  ユーザー・コンフィギュレーション情報の定義
 */

/*
 *  chg_ipmをサポートするかどうかの定義
 *　　アセンブラ用のuser_config.incにも同様の定義が必要なので注意
 */
#define SUPPORT_CHG_IPM

/*
 *  vxget_timをサポートするかどうかの定義
 */
#define SUPPORT_VXGET_TIM

/*
 *  カーネルが管理する割込みの最高優先レベル
 *　　カーネル管理下の割込み ＝ 割込みレベルがMAX_IPM 以下の割込み
 *　　カーネル管理外の割込み ＝ プライオリティレベル(MAX_IPM+1)
 *　　　　　　　　　　　　　　　以上の割込みとNMI
 *
 *　　　user_config.incにも同様の定義が必要なので注意
 */
#define MAX_IPM                 0x7u

/*
 *  シリアルポートの設定
 */

/*
 *  シリアルポート数の定義
 * 
 *　将来の拡張性のため、以下のように定義しているが、現在の実装では実質的な
 *　違いはない。
 *
 * TNUM_PORT : シリアルドライバ（serial.c）、つまり GDICレベルでサポートする
 * 　　　　　　シリアルポートの数 
 * TNUM_SIOP : PDICレベル（プロセッサ内蔵SIO）でサポートするシリアルI/Oポート
 *　　　　　　 の数（現在の実装では最大3）
 */

#define TNUM_PORT	3u
#define	TNUM_SIOP	3u	/*  現在の実装では最大3  */

/*
 *  システムタスクに関する定義
 */
#define	LOGTASK_PORTID	2	/* システムログを出力するシリアルポート番号 */

/*
 *  低レベル出力（ポーリング出力）に用いるシリアルポート番号
 */
#define	POL_PORTID	3

/*
 *  シリアルコントローラのボーレートの設定 [bps]
 */
#define BAUD_RATE0	38400	/*  SCI0  */
#define BAUD_RATE1	38400	/*  SCI1  */
#define BAUD_RATE2	38400	/*  SCI2  */
#if 0
#define BAUD_RATE0	 9600
#define BAUD_RATE1	 9600
#define BAUD_RATE2	 9600
#define BAUD_RATE0	19200
#define BAUD_RATE1	19200
#define BAUD_RATE2	19200
#endif

/*
 *  タイマの設定
 */

/*
 *  タイムティックの定義
 */

#define TIC_NUME        1u               /* タイムティックの周期の分子 */
#define TIC_DENO        1u               /* タイムティックの周期の分母 */


/*
 *  割込みレベル定義
 */
#define SCI0_INT_LVL	3u		/* SCIチャネル0の割り込みレベル */
#define SCI1_INT_LVL	3u		/* SCIチャネル1の割り込みレベル */
#define SCI2_INT_LVL	3u		/* SCIチャネル2の割り込みレベル */
#define TPU0_INT_LVL	5u		/* TPUチャネル0の割り込みレベル */

#endif /* _USER_CONFIG_H_ */
