/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 * 
 *  Copyright (C) 2005-2007 by Y.D.K.Co.,LTD Technologies company
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
 *  @(#) $Id: sys_config.h,v 1.1 2007/05/11 06:06:35 honda Exp $
 */

/*
 *  ターゲットシステム依存モジュール
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
 * 割込みハンドラの最大数
 */
#define MAX_INT_NUM 63

/*
 *  vxget_tim をサポートするかどうかの定義
 */
#define    SUPPORT_VXGET_TIM

/*
 *  シリアルポート番号に関する定義
 */
#define TNUM_PORT   2   /* サポートするシリアルポートの数 */
#define TNUM_SIOP   2   /* サポートするシリアルI/Oポートの数 */

/*
 *  シリアルポート番号に関する定義
 */
#define LOGTASK_PORTID  1   /* システムログに用いるシリアルポート番号 */

/*
 *  割込み待ち状態での実行ルーチン
 */
#define WAIT_INTERRUPT  mcr p15, 0, r3, c7, c0, 4

#ifndef _MACRO_ONLY

/*
 *  ターゲットシステム依存の初期化
 */
extern void    sys_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．ROMモニタ呼出しで実現することを想定し
 *  ている．
 */
extern void    sys_exit(void);

/*
 *  ターゲットシステムの文字出力
 *
 *  システムの低レベルの文字出力ルーチン．ROMモニタ呼出しで実現するこ
 *  とを想定している．
 */
extern void    sys_putc(char c);

/*
 *  割込みハンドラ登録用テーブル
 */
extern FP   int_table[MAX_INT_NUM];

/*
 * 割込みハンドラマスク登録用テーブル
 */
extern UW   int_mask_table[MAX_INT_NUM];

/*
 *  IRQハンドラ(sys_support.S)
 */
extern void IRQ_Handler(void);

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 */
#define INTHDR_ENTRY(inthdr)  extern void inthdr(void)

#define INT_ENTRY(inthdr) inthdr

/*
 *  割込みハンドラの設定
 *
 *  割込み番号 inhno の割込みハンドラの起動番地を inthdr に設定する．
 */
extern void define_inh(INHNO inhno, FP inthdr);

/*
 * 割込みハンドラ起動時に割込みコントローラーにセットするマスク値を設定．
 */
extern void define_inh_mask(INHNO inhno, UW mask);

/*
 * 未定義の割込みが入った場合
 */
extern void undef_interrupt();

#endif /* _MACRO_ONLY */
#endif /* _SYS_CONFIG_H_ */

