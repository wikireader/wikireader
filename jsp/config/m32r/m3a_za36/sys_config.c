/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: sys_config.c,v 1.2 2007/05/28 02:03:55 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール（M3A-ZA36用）
 */
#include <sys_rename.h>
#include <s_services.h>

#include "jsp_kernel.h"
#include <hw_serial.h>

/*
 *  ターゲットシステム依存の初期化
 */
void
sys_initialize()
{
	/* UART初期化 */
	sil_wrb_mem((void *)(SIOCR(CONSOLE_PORTID-1)+3), 0);

	/* ポートを開く(UART0) */
	sil_wrb_mem((void *)PDATA(5), 0);
	sil_wrb_mem((void *)PDIR(5),  0x80);

	/* UART, Non-parity, 1 stop-bit */
	sil_wrb_mem((void *)(SIOMOD0(CONSOLE_PORTID-1)+3), 0);

	/* 8bit, internal clock */
	sil_wrh_mem((void *)(SIOMOD1(CONSOLE_PORTID-1)+2), 0x0800);

	/* M32R(32102) - f(BLK)=10MHzで57600bps */
	sil_wrh_mem((void *)(SIOBAUR(CONSOLE_PORTID-1)+2), SERIAL_CLKDIV);
	sil_wrb_mem((void *)(SIORBAUR(CONSOLE_PORTID-1)+3), SERIAL_CLKCMP);

	/* ステータスクリア, 送受信動作開始 */
	sil_wrb_mem((void *)(SIOCR(CONSOLE_PORTID-1)+2), 0x3);
	sil_wrb_mem((void *)(SIOCR(CONSOLE_PORTID-1)+3), 0x3);

	/* TxD,RxDピン有効(UART0) */
	sil_wrh_mem((void *)PMOD(5), 0x5500);
}

/*
 *  ターゲットシステムの終了
 */
void
sys_exit(void)
{
	dis_int();
	*(volatile unsigned char *)(ICUIMASK+1) = 0;
	while(1==1)
	{
		/* 低消費電力モードへ移行 */
		__asm(	"stb	%0, @%1	\n"
				"ldb	%0, @%1 \n"
			  	"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop"
			: : "r"(1), "r"(CLKMOD+3) );
	}
}

/*
 *  ターゲットシステムの文字出力
 */
void
sys_putc(char c)
{
	if(c == '\n')
		sys_putc('\r');
	while( (sil_reb_mem((void *)(SIOSTS(CONSOLE_PORTID-1)+3)) & 0x1) == 0);	/* 送信完了待ち */
	sil_wrb_mem((void *)(SIOTXB(CONSOLE_PORTID-1)+3), c);	/* 一文字送信 */
}
