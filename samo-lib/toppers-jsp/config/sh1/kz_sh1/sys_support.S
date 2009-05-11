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
 *  @(#) $Id: sys_support.S,v 1.11 2004/09/22 08:47:52 honda Exp $
 */

/*
 *	ターゲットシステム依存モジュール アセンブリ言語部（KZ-SH1用）
 */

#define	_MACRO_ONLY
#include "jsp_kernel.h"

#ifndef GDB_STUB
/*
 *   バスステートコントローラ
 */
#define	WCR1	0x5ffffa2	/*  ウェイトコントロールレジスタ1 */
#define	WCR2	0x5ffffa4	/*  ウェイトコントロールレジスタ2 */
#define	WCR3	0x5ffffa6	/*  ウェイトコントロールレジスタ3 */

		/*  エリア7：リードサイクルでWAIT信号による  	*/
		/*  ウェートステート挿入あり  			*/
#define    WCR1_RW7	0x8000
		/*  エリア3：リードサイクルでWAIT信号による  	*/
		/*  ウェートステート挿入あり  			*/
#define    WCR1_RW3	0x800
		/*  エリア0：リードサイクルでWAIT信号による  	*/
		/*  ウェートステート挿入あり  			*/
#define    WCR1_RW0	0x100
		/*  エリア0,2：1ステートロングウェイト  	*/
#define    WCR3_A02LW	0x6000



/*
 *  低レベルのターゲットシステム依存の初期化 _hardware_init_hook
 *
 *  スタートアップモジュールの中で，メモリの初期化の前に呼び出される．
 */
	.text
	.align 2
	.global _hardware_init_hook
_hardware_init_hook:
	/*
	 *  バスステートコントローラの初期化
	 *  
	 */

	/*
	 *  ウェイトステートコントロールレジスタWCR1
	 *  　RW3 = 0
	 *  　　　エリア3：リードサイクルは１ステートで終了
	 *  
	 *  　RW0 = 0
	 *  　　　エリア0：リードサイクルでWAIT信号による
	 *  　　　ウェートステート挿入なし
	 */

			/* *WCR1 &= ~(WCR1_RW3 | WCR1_RW0); */
	mov.l	_hw_hook_wcr1, r0
	mov.w	@r0, r1			/*  r1←WCR1  */
	mov.w	_hw_hook_wcr1_data, r2
	and	r2, r1
	mov.w	r1, @r0			/*  WCR1←r1  */
	
	
	/*
	 *  ウェイトステートコントロールレジスタWCR3
	 *  
	 *  　A02LW = 00
	 *  　　　エリア0,2：1ステートロングウェイト
	 */
	
	mov.l	_hw_hook_wcr3, r3
	mov.w	@r3, r4			/*  r4←WCR3  */
	mov.w	_hw_hook_wcr3_a02lw, r5
	and	r5, r4
	rts
	mov.w	r4, @r3			/*  WCR3←r4  */

			/*  エリア0,2：1ステートロングウェイト  	*/
			/* *WCR3 &= WCR3_A02LW_MASK; */
	
	
	
	.align 2
_hw_hook_wcr1:
	.long	WCR1
_hw_hook_wcr3:
	.long	WCR3
_hw_hook_wcr1_data:	
	.word	~(WCR1_RW3 | WCR1_RW0) & 0xffff
_hw_hook_wcr3_a02lw:	
	.word	~WCR3_A02LW & 0xffff
	
#endif /* GDB_STUB */
