/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2002 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005 by Freelines CO.,Ltd 
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
 *  @(#) $Id: sys_config.h,v 1.4 2007/01/05 02:10:17 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール（TK-850/KJ1+用 外部SRAMボード付き）
 */

#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

/*
 *  カーネルの内部識別名のリネーム
 */

#include <sys_rename.h>

/*
 *  ターゲットシステムのハードウェア資源の定義
 */
#include "v850es_kj1.h"

/*
 *  起動メッセージのターゲットシステム名
 */
#define	TARGET_NAME	"TK-850/KJ1+"

/*
 *  vxget_tim をサポートするかどうかの定義
 */
/* #define	SUPPORT_VXGET_TIM */

/*
 *  JSPカーネル動作時のメモリマップと関連する定義
 */
#define	STACKTOP	(IRAM_TOP+IRAM_SIZE)	/* 非タスクコンテキスト用のスタックの初期値 */

/* CPU動作クロック周波数：20MHz(5MHz×4) */
#define	CPU_CLOCK		20000000

/* 割り込み要因の数 */
#define NUM_INTERRUPT	0x3B

/*
 *  サポートするシリアルディバイスの数は最大 2
 */
#define	CONSOLE_PORTID	1	/* コンソール用に用いるシリアルポート番号（KJ1+では1or2。3は割り込みベクターの番地が飛んでいるので不可） */
#define TNUM_PORT		1
#define SEPARATE_SIO_INT	/* 受信割り込みと送信完了割り込みが区別された割り込みになっているか */
#define	LOGTASK_PORTID		CONSOLE_PORTID	/* システムログを出力するシリアルポート番号	*/

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
Inline void
sys_putc(char c)
{
	extern void cpu_putc(char c);
	/* ROMモニタなどのコンソールへメッセージ出力する低レベル関数を想定しているが
	   面倒なのでシステムのシリアルポートへ出力する */
	cpu_putc(c);
}

#endif /* _MACRO_ONLY */
#endif /* _SYS_CONFIG_H_ */
