/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                                  Toyohashi Univ. of Technology, JAPAN
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
 *  @(#) $Id: start.S,v 1.10 2003/12/11 00:52:12 honda Exp $
 */
        
/*
 *  カーネル用のスタートアップモジュール
 */               
#define _MACRO_ONLY
#include <t_config.h>
#include <sys_defs.h>
        
/*
 *   SH3用スタートアップモジュール
 *   カーネルはアプリケーションとリンクして使用する
 *   だけなのでJSPカーネルのみ使用する。	
 */
	.global	_start
	.align 2
_start:
	/*
	 *  キャッシュの初期化
	 */
	mov.l _ccr_addr,r1
	mov.l _ccr_disable,r2
	mov.l r2,@r1
	mov.l _ccr_mode,r2
	mov.l r2,@r1

	/*
	 *  SRを初期化する。
	 *  MD=1, RB=0, BL=0, I3〜I0=e fはstubのみ
	 */
	mov.l _init_sr,r0
	ldc   r0,sr
	mov   #0x01,r0
	ldc   r0,r7_bank
	/*
	 * タスク独立のスタックをSTACKTOPに初期化する
	 * STACKTOPは、sys_config.hで定義
	 */
	mov.l  _stack_k, r15

	/*
	 *  _hardware_init_hook を呼び出す。(0でない場合)
	 *
	 *  ハードウェア依存に必要な初期化処理がある場合は，
	 *  hardware_init_hook という関数を用意すればよい．
	 *  具体的にはROM化の際、RAMを使用可能にするための
	 *  バスコントローラの初期化等を行う。
	 *  sys_support.S 内で _hardware_init_hook を定義してい
	 *  る．
	 */

start_0:
	mov.l  _hardware_init_hook_k, r0
	tst    r0,r0
	bt     start_1
	jsr    @r0
     or      r0,r0

	
	/*
     * bssセクションをクリア
	 */
start_1:		
	mov.l	_bss_start_k,r0
	mov.l	_end_k,r1
	cmp/eq  r0,r1
	bt      start_3
	mov	    #0,r2
start_2:
    mov.l	r2,@r0
	add	#4,r0
 	cmp/hi	r0,r1
	bt	start_2

	/*
	 *  dataセクションを初期化する（ROM化対応）．
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降に
	 *  コピーする．
	 */
start_3:
	mov.l   __idata_start_k,r1
	mov.l   __idata_end_k,r2
	cmp/eq  r1,r2
	bt      start_5
	mov.l   __data_start_k,r0
start_4:	
	mov.l   @r1+,r4
	mov.l   r4,@r0
	cmp/hi  r1,r2
	add     #4,r0
	bt      start_4

		
	/*
	 *  _software_init_hook を呼び出す（0 でない場合）．
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
start_5:	
	mov.l  _software_init_hook_k, r0
	tst    r0,r0
	bt     start_6		
	jsr    @r0
	or     r0,r0

		
start_6:
	/*	
     *  カーネルを起動する
	 */
	! call the mainline
	mov.l _kernel_start_k,r0
	jsr	  @r0
     or	   r0,r0


	.align 4
_ccr_addr:	
	.long CCR
_ccr_disable:	
	.long CCR_DISABLE
_ccr_mode:	
	.long CCR_MODE
_init_sr:	
	.long  0x40000000 + MAX_IPM << 4
_stack_k:	
	.long   STACKTOP
_bss_start_k:	
	.long   __bss_start	
_end_k:	
	.long  	_end	
__idata_start_k:	
	.long 	__idata_start
__idata_end_k:	
	.long   __idata_end
__data_start_k:	
	.long  	__data_start	
_kernel_start_k:	
	.long	_kernel_start
_hardware_init_hook_k:	
	.long   _hardware_init_hook
_software_init_hook_k:	
	.long   _software_init_hook	


















