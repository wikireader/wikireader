/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
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
 * 
 *  @(#) $Id: tb6102s.h,v 1.2 2004/10/07 17:10:56 honda Exp $
 */

/*
 *  タンバック　MPC860T CPUボードTB6102S
 *  のハードウェア資源の定義
 */

#ifndef _TB6102S_H_
#define _TB6102S_H_


/*
 *  割込み関連の定義
 */

/*  未定義の割込みに対するIPMのデフォルト値  */
#define DEFAULT_IPM	0x00	/*  すべての割込みを禁止  */

/*  CPMからSIUへの割込み要求レベル  */
	/*  CICR.IRL0の設定値（ビット16-18の2進数）  */
#define CICR_IRL0	(CPM_INT_LEVEL << (31 - 18))

/*  SIUから見たCPMの割込み番号  */
#define INHNO_CPM		LEVEL_TO_INHNO(CPM_INT_LEVEL)

/*  SIUから見たCPMの割込み許可ビット  */
#define ENABLE_CPM		LEVEL_TO_ENABLE_BIT(CPM_INT_LEVEL)

/*  CPM割込みのIPM  */
#define IPM_CPM			LEVEL_TO_IPM(CPM_INT_LEVEL)


/*
 *  タイマ関連の設定
 *
 */
/* タイマの割り込みレベル（SIUの内部レベル） */
#define INTLVL_PIT	INTLVL_TIMER


/*
 *  GDB STUB呼出しルーチン（未完成）
 */
#ifndef _MACRO_ONLY
#ifdef GDB_STUB

#define tb6102s_exit	stub_exit
#define tb6102s_putc	stub_putc

Inline void
stub_exit(void)
{
}

Inline void
stub_putc(char c)
{
}

/*
 *  GDB stubを使わない場合（ROM化用）
 */
#else /* GDB_STUB */

extern void	smc_putc_pol(char c);	/*  mpc860_smc.c  */

/*
 *  SILのアクセス・ログ機能を有効にした場合
 */
#ifdef SIL_DEBUG

#define SIL_BUFFER_SIZE	10000
char sil_buffer[SIL_BUFFER_SIZE];
int sil_pos;
BOOL sil_debug_on;

Inline void
tb6102s_exit()
{
	int i;

	sil_debug_on = FALSE;
	
	/*  リングバッファから古い順にデータを表示する  */
	for(i=sil_pos; i<SIL_BUFFER_SIZE; i++) {
		smc_putc_pol(sil_buffer[i]);
	}
	for(i=0; i<sil_pos; i++) {
		smc_putc_pol(sil_buffer[i]);
	}
	while(1);
}

Inline void 
tb6102s_putc(char c)
{
	++sil_pos;
	sil_pos %= SIL_BUFFER_SIZE;
	sil_buffer[sil_pos] = c;
}

#else	/* SIL_DEBUG */

Inline void
tb6102s_exit()
{
	while(1);
}

Inline void 
tb6102s_putc(char c)
{
	smc_putc_pol(c);
}

#endif	/* SIL_DEBUG */

#endif /* GDB_STUB */
#endif /* _MACRO_ONLY */
#endif /* _TB6102S_H_ */
/*  end of file  */
