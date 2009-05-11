/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2002-2004 by Hokkaido Industrial Research Institute, JAPAN
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
 *  @(#) $Id: user_config.h,v 1.3 2005/07/06 00:45:07 honda Exp $
 */

/*
 *	ユーザー定義の設定部
 */

#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/*
 *  条件コンパイル用のフラグとして用いるマクロ定義
 */

/*
 *  chg_ipm/get_ipm をサポートするかどうかの定義
 */
#define	SUPPORT_CHG_IPM

/*
 *  vxget_tim をサポートするかどうかの定義
 */
#define	SUPPORT_VXGET_TIM

/*
 *  割込みの出口処理で戻り先がCPU例外の入口処理であるかの
 *　チェックを行うどうかの定義
 */
//#define	SUPPORT_CPU_EXC_ENTRY_CHECK

/*
 *  実行すべきタスクがない場合の割込み待ちで省電力モードに移行しないか
 *　どうかの定義（フルICE向け）
 */
//#define	OMIT_POWER_CONTROL

/*
 *  スタートアップルーチンで非タスクコンテキストのスタック領域を0で
 *　初期化するどうかの定義
 */
//#define	FILL_STACK_WITH_0

/*
 *  スタートアップルーチンでdataセクションのコピーを省略するか
 *　どうかの定義
 */
//#define	OMIT_DATA_SECTION_COPY

/*
 *  JSPカーネル動作時のメモリマップ
 *      0x0000,0000 - 0x001f,ffff  コード領域(2MB)
 *      0x0400,0000 -              データ領域(1MB)
 *                  - 0x040f,ffff  タスク独立部用スタック メモリ終了
 */

/*
 *　非タスクコンテキスト用スタックの初期値
 */
#define STACKTOP    	 0x04100000


/*
 *  GDICレベルでサポートするシリアルポートの数
 *　（カーネルの管理下にあるポート数）
 */
#define TNUM_PORT 1

/*
 *  PDICレベルでサポートするシリアルポートの数
 *　（デバイスドライバ・レベルのポート数）
 */

/*
 *  システムログに用いるシリアルポート番号
 */
#define	LOGTASK_PORTID	1

/*
 *  シリアル通信のボーレート
 *　　9600, 19200, 38400[bps]のいずれかを数値で定義する。
 */
/* shxxxxsci.cで設定 */

/*
 * SCIの割込みレベル
 * 　割込みハンドラは送受信別々に登録できるが、割込みレベルは
 * 　送信も受信も同じレベルにしか設定できない。
 */
/* shxxxxsci.cで設定 */

/*
 *  タイマ割込みのレベル
 */
#define TIMER_INTLVL	4

/*
 *  タイムティックの定義
 *  　（タイマ割込みの周期：[msec]単位）
 */
#define	TIC_NUME	1u	/* タイムティックの周期の分子 */
#define	TIC_DENO	1u	/* タイムティックの周期の分母 */


/*
 *  上記のチェック対象となるCPU例外の数
 */
//#define	NUM_EXC

/*
 *  内部RAMの最終アドレス
 *　　hardware_init_hookをC言語の関数で記述するため
 *  　スタックポインタの初期値として使用される。
 */
//#define	INNER_RAM_ADDRESS


/*
 *  以下のマクロはすべて省略可能である。
 *  省略した場合はデフォルトの値として、スタートアップルーチンの
 *　先頭アドレスが設定される。
 */

/*  一般不当命令  */
//#define GII_VECTOR

/*  スロット不当命令  */
//#define SII_VECTOR

/*  CPUアドレスエラー  */
//#define CAE_VECTOR

/*  DMAアドレスエラー  */
//#define DAE_VECTOR

/*  NMI  */
//#define NMI_VECTOR


#endif /* _USER_CONFIG_H_ */
