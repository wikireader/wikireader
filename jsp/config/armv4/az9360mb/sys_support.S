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
 *  @(#) $Id: sys_support.S,v 1.2 2007/05/21 01:33:50 honda Exp $
 */

#define _MACRO_ONLY
#include "jsp_kernel.h"
#include <ns9360.h>

/* プロセッサモード */
Mode_SVC    =   0x13

/* ステータスレジスタ割り込みbit */
I_Bit       =   0x80
F_Bit       =   0x40

/*  BBUS reset register */
BBUS_RESET_BASE     = 0x90600000


/*
 *  低レベルのターゲットシステム依存の初期化
 *
 *  スタートアップモジュールの中で，メモリ初期化の前に呼び出される．          
 */       

        .text
        .align 2
        .global hardware_init_hook
hardware_init_hook: 

/*
 * NS9360依存の初期化
 */
/*
 * bbus_reset
 */
        mov     r0, #0x0
        ldr     r0, =BBUS_RESET_BASE
        mov     r1, #0x0
        str     r1, [r0, #0]

/*
 * CS、GPIO Initial
 */
        stmfd sp!, {r4 - r11,lr}   /* レジスタの保存 */
        bl      cpu_CsGpioInit
        ldmfd sp!,{r4 - r11,lr}

/*----*/
init_done:
        mov     pc, lr


/*
 *
 *  割込みの出入り口処理
 *         
 */        
        .text
        .align 4    
        .global IRQ_Handler
IRQ_Handler:

        /* 
         * 割込みモード 
         * 
         * cpsrがspsr_irqに復帰先がr14_irq(lp)に入り，
         * spsr_irqとr14_irqとr13(sp)_irqが r14,r13となる．
         */

        /* 
         *  タスクの動作時モード(スーパーバイザーモード)へ
         */
        mov     sp,#(CPSR_SVC | CPSR_FIQ_BIT | CPSR_IRQ_BIT) 
        msr     cpsr_all, sp
        stmfd   sp!, {r0-r3,ip,lr,pc} /* pcはダミー */


        /*
         * spsrと戻り番地を取得するためにIRQモードへ
         */
        mov     r0,#(CPSR_IRQ | CPSR_FIQ_BIT | CPSR_IRQ_BIT)
        msr     cpsr,r0
        sub     r0,lr,#4
        mrs     r1,spsr
        

        /*
         *  スーパーバイザーモードに
         */ 
        and     r2,  r1, #CPSR_FIQ_BIT /* FIQビットの継承 */
        orr     r2,  r2, #(CPSR_SVC|CPSR_IRQ_BIT)
        msr     cpsr, r2
        str     r0, [sp,#0x18] /* Store pc */
        stmfd   sp!,{r1}       /* spsr */


        /*
         *  多重割り込みか判定
         */
        ldr     r2, =interrupt_count
        ldr     r3, [r2]
        add     r0,r3,#1
        str     r0, [r2]
        cmp     r3, #0x00

        moveq   r2,sp            /* ネスト割り込みでない場合 */
        ldreq   sp,=STACKTOP     /* スタックの変更    */
        stmeqfd sp!,{r2}         /* タスクスタックの保存 */

        /*
         *  割り込み要因の判定．
         *  INT_IDの読み込み．(ここにある必要はない．)
         */

        ldr     r3, =ISRADDR_REG
        ldr     r0, [r3]        /* 割り込みベクタの読み出し */

        /*
         *  未定義の割込みかチェック
         */
        cmp     r0, #0x00
        beq     undefined_interrupt
        
        /* 
         * 割り込み許可 
         */
        mrs     r2, cpsr
        and     r2,  r2, #~CPSR_IRQ_BIT /* 割込み許可 */
        msr     cpsr,r2

        /* 
         * Call Handler  
         */        
        mov     lr,  pc   
        mov     pc,  r0

        /* 
         * 割り込み禁止 
         */
        mrs     r2, cpsr
        and     r2,  r2, #CPSR_FIQ_BIT /* FIQビットの継承 */         
        orr     r2,  r2, #(CPSR_SVC|CPSR_IRQ_BIT)
        msr     cpsr,r2

        /* 
         * 割込みネスト回数(interrupt_count) デクリメント 
         */
        ldr     r2, =interrupt_count
        ldr     r1, [r2]        
        sub     r3, r1, #1                        
        str     r3, [r2]
        cmp     r3, #0x00        
        bne     return_to_task_irq
        
        /*
         * 割込みネストが無いので、全てのマスクをクリア
         */
        ldr     r2,  =ISRADDR_REG
        str     r0,  [r2]       /* 割込みマスククリア */
        /* 
         * タスクスタックの復帰 
         */
        ldmfd   sp!,{r0}         
        mov     sp, r0        
                
        ldr     r1, =reqflg    /* Check reqflg */
        ldr     r0, [r1]
        cmp     r0, #0
        beq     return_to_task_irq
        mov     r0, #0
        str     r0, [r1]       /* Clear reqflg */
        b       ret_int

return_to_task_irq:
        /*
         *  復帰処理
         *  割り込み許可となるがタスクコンテキスト上に保存しているため，
         *  問題はない
         */
        ldmfd   sp!,{r1}           /* CPSRの復帰処理 */
        mrs     r2, cpsr           /* FIQを継承            */
        and     r2, r2, #CPSR_FIQ_BIT
        and     r1, r1, #~CPSR_FIQ_BIT
        orr     r1, r1, r2            
        msr     spsr, r1           
        ldmfd   sp!,{r0-r3,ip,lr,pc}^ /*タスク復帰 + 割込み許可 */

        /*
         *  未定義の割込みが入ったときに呼び出す
         */    
undefined_interrupt:
         b      undef_interrupt
    

/*
 * キャッシュ制御関係
 * UW       cpuEnableICache( void );
 * UW       cpuEnableDCache( void );
 * UW       cpuEnableMMU( UW tlbaddr );
 * UW       cpuDCache_Line_Invalid( UW mva );
 * UW       cpuDCache_Line_Flush( UW mva );
 * UW       cpuDCache_Line_FlushInalid( UW mva );
 * UW       cpuDCache_DrainWriteBuffer( void );
 */


        .global cpuEnableICache, cpuEnableDCache
        .global cpuEnableMMU
        .global cpuDCache_Line_Invalid, cpuDCache_Line_Flush
        .global cpuDCache_Line_FlushInvalid, cpuDCache_DrainWriteBuffer


#define CP15_ICACHE     0x1000
#define CP15_DCACHE     0x0004
#define CP15_MMU        0x0001


/*
 * CP15-R1のI-cache bit(b12)=1 にする。
 * 引数なし
 */
cpuEnableICache:
        mcr     p15, 0, r0, c7, c5, 0       /* ICache invalidate                */
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        mrc     p15, 0, r0, c1, c0, 0       /* CP15 R1                          */
        orr     r0, r0, #CP15_ICACHE        /* I-Cache enable                   */
        mcr     p15, 0, r0, c1, c0, 0       /* Set CP15 R1 I-cache              */
        mov     pc, lr


/*
 * CP15-R7でDcache invalidate 実施後にCP15-R1のD-cache bit(b2)=1にする。
 * 引数なし。
 */
cpuEnableDCache:
        mcr     p15, 0, r0, c7, c6, 0       /* DC all invalidate                */
        mrc     p15, 0, r0, c1, c0, 0       /* CP15 R1                          */
        orr     r0, r0, #CP15_DCACHE        /* D-Cache enable                   */
        mcr     p15, 0, r0, c1, c0, 0       /* Set CP15 R1 D-cache              */
        mov     pc,lr


/*
 * CP15-R1のMMU bit(bit0)=1にする。TLB設定後にTBLアドレスを引数にコールする。
 * r0:TLB address
 */
cpuEnableMMU:
        mcr     p15, 0, r0, c2, c0, 0       /* Set CP15 R2 TLB pointer          */
        mov     r0, #0                      /*                                  */
        mvn     r0, r0                      /* all domain is manager            */
        mcr     p15, 0, r0, c3, c0, 0       /* set Domain access (CP15 R3)      */

        mrc     p15, 0, r0, c1, c0, 0       /* CP15 R1                          */
        orr     r0, r0, #CP15_MMU           /* MMU enable                       */
        mcr     p15, 0, r0, c1, c0, 0       /* Set CP15 R1 MMU enable           */
        mov     pc,lr


/*
 * CP15-R7のInvalidate DCache single entry(MVA)を実行する。
 * r0:MVA
 */
cpuDCache_Line_Invalid:
        mcr     p15, 0, r0, c7, c6, 1       /* DC invalidate single entry(MVA)  */
        mov     pc,lr


/*
 * CP15-R7のClean DCache single entry(MVA)を実行する。
 * r0:MVA
 */
cpuDCache_Line_Flush:
        mcr     p15, 0, r0, c7, c10, 1      /* DC clean single entry(MVA)       */
        mov     pc,lr


/*
 * CP15-R7のClean & Invalidate DCache single entry(MVA)を実行する。
 * r0:MVA
 */
cpuDCache_Line_FlushInvalid:
        mcr     p15, 0, r0, c7, c14, 1      /* DC clean single entry(MVA)       */
        mov     pc,lr


/*
 * CP15-R7のDrain write bufferを実行する。
 */
cpuDCache_DrainWriteBuffer:
        mcr     p15, 0, r0, c7, c10, 4      /* Drain write buffer               */
        mov     pc,lr

