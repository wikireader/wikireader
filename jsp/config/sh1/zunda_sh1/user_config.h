/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2003-2004 by CHUO ELECTRIC WORKS Co.,LTD. JAPAN
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
 *  メモリ実装タイプがFROM,SRAMそれぞれ2種類ある
 *  FROM
 *  １．0x0000,0000 - 0x0007,ffff  コード領域(512KB)
 *  ２．0x0000,0000 - 0x0003,ffff  コード領域(256KB)
 *  SRAM
 *  １．0x0200,0000 -              データ領域(512KB)
 *                  - 0x0207,ffff  タスク独立部用スタック メモリ終了
 *  ２．0x0200,0000 -              データ領域(128KB)
 *                  - 0x0201,ffff  タスク独立部用スタック メモリ終了
 *
 *  ただしコード領域の0x0000〜0xFFFFにFROM書込みプログラム内蔵のため
 *  ユーザー使用可能領域は0x10000からとなる
 */
/*
 * SRAMのスモールタイプをご利用の場合は LARGE_TYPE をコメントアウトして下さい
 * MAKEFILEで定義しても良い
 */
//#define	LARGE_TYPE

/*
 *  値を持つマクロの定義
 */

/*
 *   スタック領域の定義
 *	非タスクコンテキスト用スタックの初期値
 */
#ifdef	LARGE_TYPE
#define STACKTOP    	0x02080000
#else
#define STACKTOP    	0x02020000
#endif	/* LARGE_TYPE */

/*
 *  クロック周波数
 */
#define	CONFIG_19MHZ	/*  19.6608MHz  */

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
#define TNUM_PORT 	2	/*  本機種では２に固定  */

/*
 *  PDICレベルでサポートするシリアルポートの数
 *　（デバイスドライバ・レベルのポート数）
 */
#define	TNUM_SIOP 	2	/*  本機種では２に固定  */

/*
 *  システムログに用いるシリアルポート番号
 */
#define	LOGTASK_PORTID	1	/*  本機種では１に固定  */

/*
 *  コンソールに用いるシリアルポート番号
 */
#define	CONSOLE_PORTID	1	/*  本機種では１に固定  */

/*
 *  シリアル通信のボーレート（ポート1）
 *　　9600, 19200, 38400[bps]のいずれかを数値で定義する。
 */
//#define	CONFIG_BAUD	9600
//#define	CONFIG_BAUD	19200
#define	CONFIG_BAUD	38400

/*
 *  シリアル通信のボーレート（ポート2）
 *　　9600, 19200, 38400[bps]のいずれかを数値で定義する。
 */
//#define	CONFIG_BAUD_2	9600
//#define	CONFIG_BAUD_2	19200
#define	CONFIG_BAUD_2	38400

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
//#define	NUM_EXC

/*
 *  内部RAMの最終アドレス
 *　　hardware_init_hookをC言語の関数で記述するため
 *  　スタックポインタの初期値として使用される。
 */
#define	INNER_RAM_ADDRESS	0x10000000

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

