/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2005 by Embedded and Real-Time Systems Laboratory
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
 */

/*
 *  プロセッサに依存する定義（Xstormy16用）
 */

#ifndef _CPU_DEFS_H_
#define _CPU_DEFS_H_

/*
 *  プロセッサ略称
 */
#define XSTORMY16

/*
 *  割込み／CPU例外ハンドラ番号の型
 */
#ifndef _MACRO_ONLY
typedef UINT INHNO;         /* 割込みハンドラ番号 */
typedef UINT EXCNO;         /* CPU例外ハンドラ番号 */
#endif /* _MACRO_ONLY */

/*
 *  プロセッサのエンディアン
 */
#define SIL_ENDIAN SIL_ENDIAN_LITTLE

/*
 *  割込みハンドラ番号
 */
#define TNUM_INTERRUPT 17   /* 割り込み要因数 */
#define INHNO_WATCHDOG 0    /* ワッチドッグタイマー割り込みハンドラ番号 */
#define INHNO_BASETIMER 1   /* ベースタイマー割り込みハンドラ番号 */
#define INHNO_TIMER0 2      /* タイマー0割り込みハンドラ番号 */
#define INHNO_TIMER1 3      /* タイマー1割り込みハンドラ番号 */
#define INHNO_UART 16       /* UART割り込みハンドラ番号 */

/*
 *  CPU例外ハンドラ番号
 */
#define TNUM_EXCEPTION 4    /* 例外要因数 */
#define EXCNO_UDINST 0      /* 未定義命令例外ハンドラ番号 */
#define EXCNO_WORDODD 1     /* 奇数番地へのワードアクセス例外ハンドラ番号 */
#define EXCNO_OUTRANGE 2    /* メモリ範囲外アクセス例外ハンドラ番号 */
#define EXCNO_UNKNOWN 3     /* 不明なCPU例外ハンドラ番号 */

#ifndef _MACRO_ONLY

/*
 *  タイムティックの定義
 */
extern UINT _kernel_tic_nume ;          /* タイムティックの周期の分子 */
#define TIC_NUME    _kernel_tic_nume    /*     (global変数として定義) */
#define TIC_DENO    1u                  /* タイムティックの周期の分母 */

/*
 *  デバッグ用ブレークポイント
 */
#define __BREAKPOINT() { __asm__( "nop" ); __asm__( ".hword 0x0007" ); }

/*
 *  stdio用低レベル入出力関数
 */
extern int _write ( int fd, char *buf, int nbytes ) ;
extern int _read ( int fd, char *buf, int nbytes ) ;

#endif /* _MACRO_ONLY */
#endif /* _CPU_DEFS_H_ */
