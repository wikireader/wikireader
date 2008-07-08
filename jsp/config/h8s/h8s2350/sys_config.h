/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
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
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */
#include <sys_rename.h>

/*
 *  システム依存のインクルードファイルの読み込み
 */
#include <h8s2350.h>
#include <mispo_h8s2350.h>

/*
 *  vgxet_tim をサポートするかどうかの定義
 */
#define	SUPPORT_VXGET_TIM

/*
 *  シリアルポート数の定義
 */
#define TNUM_PORT	2u	/* サポートするシリアルポートの数 */
#define	TNUM_SIOP	2u	/* サポートするシリアルI/Oポートの数 */

/*
 *  システムタスクに関する定義
 */
#define	LOGTASK_PORTID	1	/* システムログを出力するシリアルポート番号 */
#define	CONSOLE_PORTID	1	/* コンソール用入出力するシリアルポート番号 */
		/* CONSOLE_PORTID : monitor 向け */

/*
 *  微少時間待ちのための定義
 */
#define	SIL_DLY_TIM1	22620
#define	SIL_DLY_TIM2	3865

#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */

extern void	sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ呼出しで実現することを想定し
 *  ている．
 */

extern void	sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ呼出しで実現するこ
 *  とを想定している．
 */

extern void	sys_putc(char c) throw();

#endif /* _MACRO_ONLY */

/*============================================================================*/
/* 以下は、共通ドキュメントに無い独自の定義 */

/*
 *  JSPカーネル動作時のメモリマップ (release.ld)
 *      0x000000 - 0x01ffff     コード領域(128kB)
 *      0x200000 -              データ領域(128kB)
 *               - 0x21ffff     タスク独立部用スタック，メモリ終了
 */

/*
 *  JSPカーネル動作時のメモリマップ (debug.ld)
 *      0x200000 - 0x20ffff     コード領域(64kB)
 *      0x210000 -              データ領域(64kB-8kB(for GDB_STUB))
 *               - 0x21efff     タスク独立部用スタック，メモリ終了
 */

/*
 *   スタック領域の定義
 */
#ifndef GDB_STUB
#define STACKTOP    	0x21fffc	/* タスク独立部用スタックの初期値 */
#else /* GDB_STUB */
#define STACKTOP    	0x21effc	/* タスク独立部用スタックの初期値 */
#endif /* GDB_STUB */

#define str_STACKTOP	TO_STRING(STACKTOP)

/*
 *  割込みレベル定義
 */
#define SCI0_INT_LVL	3u		/* SCIチャネル0の割り込みレベル */
#define SCI1_INT_LVL	3u		/* SCIチャネル1の割り込みレベル */
#define TPU0_INT_LVL	5u		/* TPUチャネル0の割り込みレベル */

/*
 *  シリアルコントローラのボーレートの設定
 */
/* #define BAUD_RATE	9600u */	/* bps */
#define BAUD_RATE	38400u
/* #define BAUD_RATE	57600u */

#endif /* _SYS_CONFIG_H_ */
