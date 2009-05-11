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

#include <jsp_kernel.h>
#include <hw_serial.h>

/*
 *  ターゲットシステム依存 初期化ルーチン
 */
void
sys_initialize()
{

#ifndef GDB_STUB

	/*
	 * モジュールストップモード解除
	 */
#if TNUM_SIOP < 2
	h8s_andh( (VP) MSTPCR, ~( BIT5 ) );
#else /* TNUM_SIOP < 2 */
	h8s_andh( (VP) MSTPCR, ~( BIT6 | BIT5 ) );
#endif /* TNUM_SIOP < 2 */

	/*
	 *  バナー表示用シリアルポートの初期化
	 */
	sio_init();

#endif /* GDB_STUB */

}

/*
 *  ターゲットシステムの終了ルーチン
 */
void
sys_exit(void)
{
	while (1);
}


/*
 *  GDB STUB / 直接呼出し コンソール呼出しルーチン
 */

/* er0 に0, er1 に出力したい文字を入れ trapa #1 を実行する */
void
stub_putc(char c)
{
	/* er0 = c */
	Asm("push.l	er1");
	Asm("mov.l	er0, er1");
	Asm("sub.l	er0, er0");
	Asm("trapa	#1");
	Asm("pop.l	er1");
}

#ifdef GDB_STUB
#define h8s_putc(c)	stub_putc(c)		/* GDBスタブのシステムコール出力 */
#else  /* GDB_STUB */
#define	h8s_putc(c)	sio_snd_chr_pol(c)	/* シリアルポートへのポーリング出力 */
#endif /* GDB_STUB */

/*   print_banner()  */
void
sys_putc(char c)
{
	if (c == '\n') {
		h8s_putc('\r');
	}
	h8s_putc(c);
}
