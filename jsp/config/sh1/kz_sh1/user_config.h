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
 *  @(#) $Id: user_config.h,v 1.4 2005/11/07 01:15:43 honda Exp $
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
#if 0
#define	SUPPORT_CPU_EXC_ENTRY_CHECK
#endif

/*
 *  実行すべきタスクがない場合の割込み待ちで省電力モードに移行しないか
 *　どうかの定義（フルICE向け）
 */
#if 0
#define	OMIT_POWER_CONTROL
#endif

/*
 *  スタートアップルーチンで非タスクコンテキストのスタック領域を0で
 *　初期化するどうかの定義
 */
#if 0
#define	FILL_STACK_WITH_0
#endif

/*
 *  スタートアップルーチンでdataセクションのコピーを省略するか
 *　どうかの定義
 */
#if 0
#define	OMIT_DATA_SECTION_COPY
#endif

/*
 *  SRAMメモリ実装タイプをラージタイプとするかどうかの定義
 *　（中央製作所製 SH1CPUボード固有なので、本機種では未使用）
 */
#if 0
#define	LARGE_TYPE
#endif


/*
 *  値を持つマクロの定義
 */


/*
 *  JSPカーネル動作時のメモリマップ
 *      0x0000,0000 - 0x0004,0000  コード領域(256KB)
 *      0x0a00,0000 -              データ領域(256KB)
 *                  - 0x0a03,ffff  タスク独立部用スタック メモリ終了
 */

/*
 *　非タスクコンテキスト用スタックの初期値
 */
#define STACKTOP    	0x0a03fffc

/*
 *  クロック周波数
 *　　いずれか１つを選択する。
 *　　　・京都マイクロコンピュータ(株) KZ-SH1-01　20MHz
 *　　　・CQ出版 RISC評価キット SH-1　            19.6608MHz
 *　　　・ソフィアシステムズ製MultiSTAC-SH1       20MHz
 */
#define	CONFIG_20MHZ
#if 0
#define	CONFIG_19MHZ	/*  19.6608MHz  */
#define	CONFIG_16MHZ
#endif

/*
 *  シリアルポート数の定義
 *
 *  TNUM_PORTはシリアルドライバ（serial.c）、つまり GDICレベルでサポー
 *  トしているシリアルポートの数であり、機種依存部で定義するよう共通部
 *  とのインターフェースで規定されている。
 *  一方、TNUM_SIOPはPDICレベルでサポートしているシリアルポートの数で
 *  あり、機種依存部の中でのみ用いている。
 *　
 */

/*
 *  GDICレベルでサポートするシリアルポートの数
 *　（カーネルの管理下にあるポート数）
 */
#define TNUM_PORT 1	/*  本機種では１に固定  */

/*
 *  PDICレベルでサポートするシリアルポートの数
 *　（デバイスドライバ・レベルのポート数）
 */
#define	TNUM_SIOP 1	/*  本機種では１に固定  */

/*
 *  システムログに用いるシリアルポート番号
 */
#define	LOGTASK_PORTID	1  /*  本機種では１に固定  */

/*
 *  シリアル通信のボーレート
 *　　9600, 19200, 38400[bps]のいずれかを数値で定義する。
 */
#if 0
#define	CONFIG_BAUD	9600
#define	CONFIG_BAUD	19200
#endif

#define	CONFIG_BAUD	38400

/*
 * SCIの割込みレベル
 * 　割込みハンドラは送受信別々に登録できるが、割込みレベルは
 * 　送信も受信も同じレベルにしか設定できない。
 */
#define SCI_INTLVL	6	/*  送受信共用  */

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
#if 0
#define	NUM_EXC
#endif

/*
 *  内部RAMの最終アドレス
 *　　hardware_init_hookをC言語の関数で記述するため
 *  　スタックポインタの初期値として使用される。
 */
#if 0	/*  本機種は内蔵RAMを持たない  */
#define	INNER_RAM_ADDRESS
#endif


/*
 *  以下のマクロはすべて省略可能である。
 *  省略した場合はデフォルトの値として、スタートアップルーチンの
 *　先頭アドレスが設定される。
 */

/*  一般不当命令  */
#if 0
#define GII_VECTOR
#endif

/*  スロット不当命令  */
#if 0
#define SII_VECTOR
#endif

/*  CPUアドレスエラー  */
#if 0
#define CAE_VECTOR
#endif

/*  DMAアドレスエラー  */
#if 0
#define DAE_VECTOR
#endif

/*  NMI  */
#if 0
#define NMI_VECTOR
#endif


#endif /* _USER_CONFIG_H_ */
