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
 *  @(#) $Id: ms7709ase01.h,v 1.4 2003/12/11 06:47:39 honda Exp $
 */

/*
 *   MS7729RSE01用の定義
 *   sys_defs.h からインクルードする
 */

#ifndef _MS7709ASE01_H_
#define _MS7709ASE01_H_

/*
 *  プロセッサのエンディアン
 */
#define SIL_ENDIAN  SIL_ENDIAN_LITTLE  /* リトルエンディアン */

/*
 * プロセッサの種類(DSPには対応していない)
 */
#define SH3
#define SH7709A

/*
 *  起動メッセージのターゲット名
 */
#define	TARGET_NAME	"MS7709ASE01(CPU:SH7709A Solution Engine)"

/*
 *  キャッシュモードの設定
 *  P0,U0,P1ライトスルーモード
 */
#define CCR_MODE      0x00000003  

/*
 *  SCIF,SCI,TMUの設定値
 *
 */

/*
 * カーネルではCPGの設定は行わない
 * GDB_STUBもしくはICEにより
 * クロックモード0 
 * OSC1 33MHz : I:B:P = 4:1:1
 * 設定すること
 * 
 */

/*
 *  タイマ値の内部表現と msec 単位との変換
 *  Pクロックは 33.2MHz
 *  分周比 /4,/16,/64/256 のいずれかを選択
 *  /4:  8.3Mhz   tick 8300   0.12 μsec
 *  /16: 2.075Mhz tick 2075   0.48 μsec 
 *  /64: 0.518Mhz tick 518    1.93 μsec
 *  /256:0.129Mhz tick 129    7.73 μsec
 *
 */
#define TCR0_TPSC  0x0001


/*
 *  タイマ値の内部表現とミリ秒単位との変換
 */
#define TIMER_CLOCK		2075

/*
 *  スーパーI/O(FDC37C935A)のベースアドレス
 */
#define SMSC_BASE_ADDR 0xB0400000

/*
 *  スーパーI/O(FDC37C935A)上のSCI0のボーレートの設定値
 *  19200bps
 */
#define SMSC_SCI0_BPS 0x0006

/*
 *  微少時間待ちのための定義
 */
#define	SIL_DLY_TIM1	 90
#define	SIL_DLY_TIM2	 50

#endif  /* _MS7709ASE01_H_ */

