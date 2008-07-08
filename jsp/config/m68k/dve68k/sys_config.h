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
 *  @(#) $Id: sys_config.h,v 1.10 2003/12/06 09:47:04 hiro Exp $
 */

/*
 *	ターゲットシステム依存モジュール（DVE68K/40用）
 *
 *  このインクルードファイルは，t_config.h のみからインクルードされる．
 *  他のファイルから直接インクルードしてはならない．
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
#include <dve68k.h>

/*
 *  起動メッセージのターゲットシステム名
 */
#define	TARGET_NAME	"DVE-68K/40"

/*
 *  vxget_tim をサポートするかどうかの定義
 */
#define	SUPPORT_VXGET_TIM

/*
 *  JSPカーネル動作時のメモリマップと関連する定義
 *
 *	00000000 - 0000ffff	ROMモニタ／スタブワークエリア (64KB)
 *	00010000 - 000fffff	コード領域 (約1MB)
 *	00100000 -		データ領域 (約3MB)
 *		 - 003fffff	非タスクコンテキスト用のスタック領域
 */
#define	STACKTOP 0x00400000	/* 非タスクコンテキスト用のスタックの初期値 */

/*
 *  シリアルポート数の定義
 */
#define TNUM_PORT	2	/* サポートするシリアルポートの数 */
#define	TNUM_SIOP	2	/* サポートするシリアルI/Oポートの数 */

/*
 *  シリアルポート番号に関する定義
 */
#define	LOGTASK_PORTID	1	/* システムログに用いるシリアルポート番号 */

/*
 *  シリアルポートのボーレートに関する定義
 *
 *  μPD72001のボーレートジェネレータに設定すべき値．
 */
#define BRG1_DEF	0x00u		/* 9600bps（上位）*/
#define BRG2_DEF	0x1eu		/* 9600bps（下位）*/

/*
 *  微少時間待ちのための定義
 */
#define	SIL_DLY_TIM1	420
#define	SIL_DLY_TIM2	90

#ifndef _MACRO_ONLY
/*
 *  プロセッサ識別のための変数（マルチプロセッサ対応）
 */
extern UINT	board_id;	/* ボードID */
extern VP	board_addr;	/* ローカルメモリの先頭アドレス */

/*
 *  ターゲットシステム依存の初期化
 */
extern void	sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ／GDB STUB呼出しで実現する．
 */
extern void	sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ／GDB STUB呼出しで実
 *  現する．
 */
extern void	sys_putc(char c);

#endif /* _MACRO_ONLY */
#endif /* _SYS_CONFIG_H_ */
