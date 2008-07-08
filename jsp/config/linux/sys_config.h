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
 *  @(#) $Id: sys_config.h,v 1.7 2003/12/24 07:24:40 honda Exp $
 */

#ifndef _SYS_CONF_
/*
 *  OS の種類の定義
 */

/*
 *  起動メッセージのターゲット名
 */
#define	TARGET_NAME	"Linux"

/*
 *  CPU と _setjmp/_longjmp ライブラリの実装に依存する定数の設定
 *
 *  JMPBUF_PC: jmp_buf 構造体中で，PC の値を入れる場所のオフセット
 *  JMPBUF_SP: jmp_buf 構造体中で，SP の値を入れる場所のオフセット
 *
 *  STACK_MERGIN: スタックポインタの初期値を，スタック領域の上限 (確保
 *  した領域の次の番地) からど れだけ離すか．スタックポインタが指す番
 *  地から使い始めるプロセッサの の場合には，これを使ってスタックポイ
 *  ンタの初期値を上限から下げる．また，longjmp した後に，スタックポイ
 *  ンタより上の番地を アクセスする可能性がある場合には，これを使って
 *  マージン領域を確保する．
 *
 *  SIGSTACK_MERGIN: シグナルスタックをプロセススタック上に取る時のマー
 *  ジン．スタートアップルーチンが使うためのスタック領域．
 */

#if defined(i386) | defined(_i386_) | defined(__i386__)

#define JMPBUF_PC		JB_PC
#define JMPBUF_SP		JB_SP
#define STACK_MERGIN		4
#define SIGSTACK_MERGIN		8192

#else /* i386 */

#error not supported.

#endif /* i386 */

/*
 *  シグナルスタックの変更に sigaltstack を使う
 */
#define USE_SIGALTSTACK

/*
 *  スタックエリアの取得/返却関数を mprotect を使うものに差しかえる
 */
#define	USE_MPROTECT_STACK



#ifdef _BSD_SERIAL_
#undef RAW			/* 衝突するシンボルの定義を抹消 */
#undef ECHO
#endif /* _BSD_SERIAL_ */


/*
 *  システムタスクに関する定義
 */
#define	CONSOLE_PORTID	1	/* コンソール用に用いるシリアルポート番号 */
#define	LOGTASK_PORTID	1	/* システムログを出力するシリアルポート番号 */
#define LOGTASK_STACK_SIZE 8192 /* ログタスクのスタックサイズ */

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
extern void	sys_putc(char c);

#endif /* _MACRO_ONLY */
#endif /* _SYS_CONF_ */



