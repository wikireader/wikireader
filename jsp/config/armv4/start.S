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
 *  @(#) $Id: start.S,v 1.19 2007/01/05 02:02:38 honda Exp $
 */
                
/*
 *    カーネル用のスタートアップモジュール(ARMv4用)
 */
#define _MACRO_ONLY
#include <armv4.h>
#include <sys_defs.h>
        
#include <t_config.h>
 


        /*
         * Vectorテーブルとジャンプテーブル
         * サイズは合計0x3c
         */
        .section .vector,"a"
        .code 32
        .align 0
        .global vector_table
vector_table:
        ldr pc, reset_vector       /* リセット             */
        ldr pc, undef_vector       /* 未定義命令           */
        ldr pc, swi_vector         /* ソフトウェア割込み   */
        ldr pc, prefech_vector     /* プリフェッチアボード */
        ldr pc, data_abort_vector  /* データアボード       */
        ldr pc, reset_vector
        ldr pc, irq_vector         /* IRQ                  */
        ldr pc, fiq_vector         /* IRQ                  */

/*
 *  ベクターテーブル
 */
reset_vector:
        .long   start
undef_vector:
        .long   undef_exception
swi_vector:
        .long   swi_exception
prefech_vector:
        .long   prefetch_exception
data_abort_vector:
        .long   data_abort_exception
irq_vector:
        .long   irq_exception
fiq_vector:
        .long   fiq_exception




        .text
        .code 32
        .align     0
        .global    start
start:
        mov  r1,  #(CPSR_SVC|CPSR_FIQ_BIT|CPSR_IRQ_BIT) /* To System mode */
		msr  cpsr, r1

        /*
         * スタックポインタと，フレームポインタの初期化
         * SL(スタック上限レジスタr10)は設定する必要があるか?
         * gccはディフォルトでは-mapcsなので必要ない
         */
        ldr  r3, =STACKTOP
        mov  sp, r3
        mov  r11, #0        /* Armモード用フレームポインタ   */         
        mov  r7, #0         /* Thubmモード用フレームポインタ */

        /*
         * hardware_init_hookの呼び出し （0でない場合）
         *
         * ターゲットハードウェアに依存して必要な初期化処理がある場合
         * は，hardware_init_hook という関数を用意する
         */
        ldr  r0, =hardware_init_hook
        cmp  r0, #0x00
        blne hardware_init_hook


        /*
         *  bssセクションのクリア
         */        
start_1:
        ldr  r1, =__bss_start
        ldr  r2, =__bss_end
        cmp  r1,r2
        bhs  start_3
        mov  r0,#0
start_2:
        str  r0,[r1],#4
        cmp  r1, r2
        blo  start_2
        
        /*
         *  dataセクションの初期化(ROM化対応)
         *  __idata_start から __idata_end までを，__data_start 以降に
         *  コピーする
         */
start_3:
#if 0
        ldr  r1, =__idata_start
        ldr  r3, =__idata_end
        cmp  r1, r3
        bhs  start_5
        ldr  r2, =__data_start
start_4:        
        ldr  r0, [r1],#4
        str  r0, [r2],#4
        cmp  r1, r3
        blo  start_4
#endif
        
        /*
         *  software_init_hook を呼出し（0 でない場合）
         *
         *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
         *  理がある場合は，software_init_hook という関数を用意すれば
         *  よい．
         */
start_5:
        ldr   r0, =software_init_hook
        cmp   r0, #0x00
        blne  software_init_hook

start_6:        
        bl    kernel_start
