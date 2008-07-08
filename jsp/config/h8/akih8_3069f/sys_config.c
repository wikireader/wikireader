/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2001-2007 by Industrial Technology Institute,
 *                              Miyagi Prefectural Government, JAPAN
 *  Copyright (C) 2001-2004 by Dep. of Computer Science and Engineering
 *                   Tomakomai National College of Technology, JAPAN
 *  Copyright (C) 2001-2004 by Kunihiko Ohnaka
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
 *  @(#) $Id: sys_config.c,v 1.12 2007/03/23 07:22:15 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール
 */

#include "jsp_kernel.h"
#include <sil.h>

/*
 *  ターゲットシステム依存 初期化ルーチン
 */

void
sys_initialize(void)
{
}

/*
 *  ターゲットシステムの終了ルーチン
 */

void
sys_exit(void)
{
  while (1)
  	;
}

#ifdef REDBOOT
/*
 *  Redbootが使用する割込みベクタの退避と復元
 */
/*  割込みベクタの退避  */
void
load_vector(TMP_VECTOR *p) {
	UW *redboot_trap_vector = (UW *)VECTOR_TABLE_ADDR + TRAP8;
	UW *pirq5 = (UW *)VECTOR_TABLE_ADDR + IRQ_EXT5;
	
	/* ベクタテーブルの内、trapa命令分を退避 */
	memcpy(&(p->trap_vector), redboot_trap_vector, TRAP_VECTOR_SIZE*4);

	/*  IRQ5:Ethernet割込みの先頭アドレスを退避  */
	p->irq5 = sil_rew_mem((VP)pirq5);		
}

/*  割込みベクタの復元  */
void
save_vector(TMP_VECTOR *p) {
	UW *redboot_trap_vector = (UW *)VECTOR_TABLE_ADDR + TRAP8;
	UW *pirq5 = (UW *)VECTOR_TABLE_ADDR + IRQ_EXT5;
	
	/*  IRQ5:Ethernet割込みの先頭アドレスを復元  */
	sil_wrw_mem((VP)pirq5, p->irq5);		

	/* ベクタテーブルの内、trapa命令分を復元 */
	memcpy(redboot_trap_vector, &(p->trap_vector), TRAP_VECTOR_SIZE*4);
}

#endif	/* of #ifdef REDBOOT */

