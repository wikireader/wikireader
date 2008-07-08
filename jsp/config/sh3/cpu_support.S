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
 *  @(#) $Id: cpu_support.S,v 1.37 2005/08/22 16:57:25 honda Exp $
 */


#define _MACRO_ONLY
#include "jsp_kernel.h"
#include "offset.h"
#include <t_config.h>
        
 			
/*
 *  例外/割り込み出入り口処理ルーチン
 *  SH3はベクタテーブルを持たず、例外,TRAPAが発生するとVBR+0x100番地を、
 *  内部/外部/NMI割り込みが発生すると、VBR+0x600番地を実行する。
 */
        
/*
 *	VBR に設定する値
 */
    .section .text
    .align 2        
    .global _BASE_VBR
_BASE_VBR:   
     nop 
     nop
     nop

/*
 *  Exception 100 vector
 *  アドレスエラー,スロット不当命令例外、Trapa例外等が発生すると実行される。
 *
 *  CPU例外ハンドラは、非タスクコンテキストで実行する、そのため、CPU例外ハン
 *  ドラを呼び出す前に例外/割り込みを示すレジスタバンク1の r7 レジスタをイン
 *  クメントし、リターンしてきた後にデクリメントする。CPU例外がタスクコンテキ
 *  ストで発生し、regflg が TRUE になった時に、 ret_exc へ分岐する。
 *  regflg をチェックする前に割り込みを禁止しないと、reqflg をチェック後に起
 *  動された割り込みハンドラ内でディスパッチが要求された場合に、ディスパッチ
 *  されない。
 *
 *		
 *  例外が発生すると、例外発生元のコンテキストであるspc,pr,ssr,r0〜r7を退避
 *  する。例外が発生した時点でのコンテキストを調べ非タスクコンテキストなら
 *  _exception_from_intから実行を開始する。例外の要因はEXPEVTレジスタ読み、
 *  exc_tableからCPU例外ハンドラの開始番地を読み込む。発生した例外に対して
 *  CPU例外ハンドラが登録されていなければ、_no_reg_exceptionにブランチして、
 *  ssr,sr,spc,prの内容を出力して停止する。CPU例外ハンドラからリターンして
 *  きたら、reqflgを調べ0以外ならrec_exc()に飛び、0なら元の処理にリターンす
 *  る。
 *
 *
 *  BLビットの扱いについて
 *  ssr,spc に戻り先のコンテキストを戻して rte を発行する際には，割込み（NMI 
 *  も含む）を受け付けないように BL ビットを1にする必要がある．例外/割込みハ
 *  ンドラ終了時には BL をビットを1にせず IPM を MAX_IPM に設定する．これは，
 *  BL ビットが1の場合に例外が発生すると CPU がリセットされることを考慮して，
 *  安全のため，ssr,spcを復帰する直前でBLを1に設定している．
 *       
 */	
			

    .org   0x0100              /*  _BASE_VBR + 0x0100番地に配置   */
    .align 2
    .global _general_exception
_general_exception:
    stc.l  spc,@-r15           /* spc,pr,ssr をスタックに保存する */
    sts.l  pr,@-r15               
    stc.l  ssr,@-r15
    stc.l  gbr,@-r15					
    sts.l  macl,@-r15					
    sts.l  mach,@-r15					                        
    stc.l  r0_bank,@-r15       /* 例外発生時のr0〜r7を保存        */
    stc.l  r1_bank,@-r15
    stc.l  r2_bank,@-r15
    stc.l  r3_bank,@-r15
    stc.l  r4_bank,@-r15
    stc.l  r5_bank,@-r15
    stc.l  r6_bank,@-r15    
    stc.l  r7_bank,@-r15
    tst    r7,r7               /* 例外発生元のコンテキストを判定            */
    bf/s   _exception_from_exc /* 発生元が例外/割り込みハンドラならジャンプ */
     add    #0x01,r7	       /* 例外/割り込みネスト回数をインクリメント   */
    mov    r15,r1              /* 戻り先が割り込みハンドラでないのでスタッ  */
    mov.l  _stacktop_exc,r15   /* クを入れ替え元のスタックポインタを保存    */ 
    mov.l  r1,@-r15 
    mov.l  _expevt,r0          /* 例外を許可すると消えるレジスタを保存      */
    mov.l  @r0,r5              /* EXPEVTレジスタを取得                      */
    ldc    r1,r4_bank          /* CPU例外ハンドラへの引数                   */
    ldc    r5,r5_bank	     
    mov.l  _unmak_rb_bl_exc,r2 /* BLビットを0に，レジスタバンクを切り替え   */
    stc    sr,r3               
    and    r3,r2			
    ldc    r2,sr               
    shlr2  r5                  /* EXPEVTを3ビット右シフト                   */
    shlr   r5                
    mov.l  _exc_table_exc,r0   /* exc_tableのアドレスをr0に                 */
    mov.l  @(r0,r5),r2         /* ハンドラのアドレスを取得                  */
    tst    r2,r2               /* ハンドラが登録されていない例外なら        */
    bt     _no_reg_exception   /* no_reg_exceptionへジャンプ                */
    jsr    @r2                 /* CPU例外ハンドラへ                         */
     nop
    mov.l  _mask_md_ipm_exc,r0
    ldc    r0,sr               /* 割り込み禁止                              */
    stc    r7_bank,r7          /* 例外/割り込みのネスト回数をデクリメント   */
    dt     r7
    ldc    r7,r7_bank
    mov.l  _reqflg_exc,r2      /* reqflgのチェック                          */
    mov.l  @r2,r1
    tst    r1,r1               /* reqflgがFALSEならret_to_task_excに飛び    */
    bt/s   _ret_to_task_exc    /* タスクに戻る                              */
     mov.l  @r15,r15           /* 戻り先がタスクなのでスタックを戻す        */
    xor    r0,r0               
    bra    ret_exc
     mov.l  r0,@r2             /* reqflgをクリア                            */


/*
 *   exception_from_int
 *   CPU例外発生時のコンテキストが非タスクコンテキストの場合の処理
 *   タスクコンテキストの場合との処理の違いは、とCPU例外ハンドラの
 *   処理が終了すると必ず元の処理に戻ることである。
 */
	
_exception_from_exc:
    mov.l   _expevt,r0        /* 例外を許可すると消えるレジスタを保存      */
    mov.l   @r0,r5            /* EXPEVTレジスタを取得                      */
    ldc     r15,r4_bank       /* CPU例外ハンドラへの引数                   */
    ldc     r5,r5_bank	     
    mov.l   _unmak_rb_bl_exc,r2 /* BLビットを0に，レジスタバンクを切り替え */
    stc     sr,r3
    and     r3,r2			
    ldc     r2,sr               
    shlr2   r5                /* EXPECTを3ビット右シフト                   */
    shlr    r5                	
    mov.l   _exc_table_exc,r0 /* exc_tableのアドレスをr0に                 */
    mov.l   @(r0,r5),r2       /* ハンドラのアドレスを取得                  */
    tst     r2,r2             /* ハンドラが登録されていない例外なら        */
    bt      _no_reg_exception /* no_reg_exceptionへジャンプ                */
    jsr     @r2               /* CPU例外ハンドラへ                         */
     nop
    mov.l  _mask_md_ipm_exc,r0
    ldc    r0,sr              /* BL=1,割り込み禁止                         */
    stc    r7_bank,r7         /* 例外/割り込みのネスト回数をデクリメント   */
    dt     r7
    ldc    r7,r7_bank
_ret_to_task_exc:
    mov.l  @r15+,r7           /* r0〜r1,ssr,pr,spcを復帰しタスクに戻る     */
    mov.l  @r15+,r6
    mov.l  @r15+,r5
    mov.l  @r15+,r4
    mov.l  @r15+,r3
    mov.l  @r15+,r2
    mov.l  @r15+,r1        
    mov.l  _mask_md_bl_exc,r0 
    ldc    r0,sr             /* BLを1に                                   */
    mov.l  @r15+,r0
    lds.l  @r15+,mach                
    lds.l  @r15+,macl        
    ldc.l  @r15+,gbr               
    ldc.l  @r15+,ssr            
    lds.l  @r15+,pr
    ldc.l  @r15+,spc
    rte
     nop


/*	
 *  no_reg_exception()
 *  CPU例外として登録されていない例外が発生すると呼び出される
 *  例外コード、例外が発生した時点のpc,sr,prを出力してカーネル
 *  を停止する。
 */					
_no_reg_exception:
    mov.l  _expevt,r5
    mov.l  @r5,r4
    stc    spc,r5
    stc    ssr,r6
    sts    pr,r7
    mov.l  _cpu_expevt_k,r1
    jsr    @r1
     nop

	.align  4
_stacktop_exc:	
    .long  STACKTOP            /* タスク独立部のスタックの初期値  */
_expevt:	
    .long  EXPEVT
_exc_table_exc:	
    .long  _exc_table
_unmak_rb_bl_exc:		
    .long  0x4fffffff
_mask_md_ipm_exc:	
    .long  0x40000000 + MAX_IPM << 4	
_reqflg_exc:
    .long  _reqflg
_cpu_expevt_k:	
    .long  _cpu_expevt	
_mask_md_bl_exc:	
    .long  0x50000000 + MAX_IPM << 4	



	
/*
 *  タスクディスパッチャ
 *
 *  _dispatch は、r7_bank1 = 0,割込み禁止状態で呼び出さなければならな
 *  い．_exit_and_dispatch も，r7_bank1 = 0・割込み禁止状態で呼び出す
 *  のが原則であるが，カーネル起動時に対応するため，r7_bank1 = 1で呼び
 *  出した場合にも対応している．
 */

    .text
    .align 2
    .global _dispatch
_dispatch:
    sts.l  pr,@-r15         /* pr,r8〜r15 をスタックに保存           */
    mov.l  r14,@-r15        /* r0〜r7は呼び出し元で保存しているため  */    
    mov.l  r13,@-r15        /* 保存する必要が無い                    */    
    mov.l  r12,@-r15
    mov.l  r11,@-r15
    mov.l  r10,@-r15
    mov.l  r9, @-r15
    mov.l  r8, @-r15
    mov.l  _runtsk_dis,r2   /* r0 <- runtsk                          */
    mov.l  @r2,r0
    mov.l  r15,@(TCB_sp,r0) /* タスクスタックをTCBに保存 60以下ならOK*/		
    mov.l  dispatch_r_k,r1  /* 実行再開番地を保存                    */
    mov.l  r1,@(TCB_pc,r0)  /* 実行再開番地をTCBに保存  60以下ならOK */
    bra    dispatcher
     nop

dispatch_r:	
    mov.l  @r15+,r8         /* レジスタを復帰                       */
    mov.l  @r15+,r9
    mov.l  @r15+,r10
    mov.l  @r15+,r11
    mov.l  @r15+,r12
    mov.l  @r15+,r13
    mov.l  @r15+,r14
    lds.l  @r15+,pr
    /*
     *  タスク例外処理ルーチンの起動
     *  dispatch_rはdispatcherから呼び出されるため，
     *  tcbのアドレスはr0に入っている．      
     */
    mov.l  @(TCB_enatex,r0),r1
    mov.l  _enatex_mask_dis,r2
    tst    r1,r2
    bt     dispatch_r_1         /* enatex が FALSE ならリターン */
    mov.l  @(TCB_texptn,r0),r1  /* texptnをロード               */
    tst    r1,r1
    bt     dispatch_r_1         /* texptn が 0で無ければ        */
    mov.l  _call_texrtn_dis,r1  /* タスク例外ルーチンの呼び出し */
    jmp    @r1        
     nop                          
dispatch_r_1:   
    rts
     nop                

    .global _exit_and_dispatch
_exit_and_dispatch:
    xor    r1,r1                 /* r7_bank1を0クリア             */
    ldc    r1,r7_bank
dispatcher:
    /*
     * ここには割り込み禁止で来ること
     */
    mov.l  _schedtsk_dis,r12    /* r0 <- schedtsk                   */
    mov.l  @r12,r0             
    mov.l  _runtsk_dis,r2
    mov.l  r0,@r2               /* schedtskをruntskに               */
    cmp/eq #0,r0                /* schedtsk があるか？              */
    bt     dispatcher_1         /* 無ければジャンプ                 */        
    mov.l  @(TCB_sp,r0),r15     /* TCBからタスクスタックを復帰      */
    mov.l  @(TCB_pc,r0),r1      /* TCBから実行再開番地を復帰        */
    jmp    @r1 
     nop
dispatcher_1:
    /* イミディエイト値を毎回読み込むのは効率が悪いので事前に用意する
     * r8  : 割り込み禁止のためのSRの値   
     * r9  : 割り込み待ち中のSRの値
     * r10 : reqflgの番地
     * r11 : 0x01
     * r12 : 0x00
     * r13 : sleep中のSRの状態       
     */
    mov.l  _mask_md_ipm_dis,r8  /* 割り込み禁止のSRの値             */
#ifdef NO_SLEEP
    mov.l  _mask_md_dis,r9      /* 割り込み待ち中のSRの値           */        
#else
    mov.l  _mask_bl_md_dis,r9   /* 割り込み待ち中のSRの値           */
#endif                          
#ifdef SUPPORT_CHG_IPM
    mov.l  _task_intmask_k,r2
    mov.l  @r2,r3
    or     r3,r9
#endif
    mov.l  _reqflg_dis,r10
    mov    #0x01,r11
    xor    r12,r12
    mov.l  _stacktop_dis,r15   /* スタックを割り込みスタックに     */
    ldc    r11,r7_bank         /* r7_bank1 を1にして割り込み状態に */        
dispatcher_2:        
    /*
     *  ここで割込みモードに切り換えるのは，ここで発生する割込み処理
     *  にどのスタックを使うかという問題の解決と，割込みハンドラ内で
     *  のタスクディスパッチの防止という二つの意味がある．
     */
     /*
      *  割込み許可後にsleep命令を実行すると，割込み許可命令の実行前に
      *  割込み要求が入っている場合，割込み許可命令の実行と共に割込みが
      *  受け付けられその復帰後に，reqflgのチェックではなく sleep 状態
      *  に入ってしまう．
      *  この問題は，割込みの許可と sleep 状態への移行がアトミックに実
      *  行できないことに起因する．
      *  そのため，sleep 実行直前の SR 変更では BL ビットを'1'に設定す
      *  ることで，割込みの許可と sleep 状態への以降をアトミックに実行
      *  する．
      *  ldc r9,sr ではBLビットがセットされるため，割込みは受け付けずス
      *  リープモードに移行する．スリープモード中は，BLビットが1でも割
      *  込みを受け付けるため(SH7709ハードウェアマニュアル 8-10)，スリー
      *  プモードへの移行直後に割込みを受付スリープモードから復帰する．
      */
               
    ldc    r9,sr               /* 割り込みの許可                   */
#ifndef NO_SLEEP
    sleep                      /* 割込み待ち                       */
#endif /* NO_SLEEP */                
    ldc    r8,sr               /* 割り込み禁止                     */  
    mov.l  @r10,r7             /* r7 <- reqflg                     */
    tst    r7,r7               /* reqflg が FALSEなら              */
    bt     dispatcher_2        /* dispatcher_2 へ                  */
    ldc    r12,r7_bank         /* r7_bank1 をクリア                */        
    mov.l  r12,@r10            /* reqflg を FALSEに                */
    bra    dispatcher
     nop	
    .align 4
_runtsk_dis:	
    .long _runtsk
_schedtsk_dis:	
    .long _schedtsk
_enatex_mask_dis:    
    .long TCB_enatex_mask
_call_texrtn_dis:	
    .long _call_texrtn
_mask_md_ipm_dis:	
    .long 0x40000000 + MAX_IPM << 4
_mask_md_dis:	
    .long 0x40000000
_mask_bl_md_dis:	
    .long 0x50000000	        
dispatch_r_k:	
    .long dispatch_r
_stacktop_dis:	
    .long STACKTOP            /* タスク独立部のスタックの初期値	*/
_reqflg_dis:
    .long  _reqflg

/* 
 *  割り込みハンドラ/CPU例外ハンドラ出口処理
 *
 * 戻り先がタスクでreqflgがセットされている場合のみここにくる。
 * r7_bank1 = 0,割り込み禁止状態,スクラッチレジスタを保存した
 * 状態で呼び出すこと。 
 *	
 */
    .text
    .align 2
    .globl ret_int
    .globl ret_exc
ret_exc:
ret_int:
    mov.l  _runtsk_ret,r1     /* r0 <- runtsk                  */
    mov.l  @r1,r0           
    mov.l  _enadsp_ret,r2     /* enadspのチェック              */
    mov.l  @r2,r3
    tst    r3,r3           
    bt     ret_int_1           
    mov.l  _schedtsk_ret,r4   /* r5 <- schedtsk                */ 
    mov.l  @r4,r5
    cmp/eq r0,r5              /* runtsk と schedtsk を比較     */
    bt     ret_int_1
    mov.l  r14,@-r15          /* 残りのレジスタを保存          */
    mov.l  r13,@-r15					
    mov.l  r12,@-r15					
    mov.l  r11,@-r15					
    mov.l  r10,@-r15					
    mov.l  r9,@-r15					
    mov.l  r8,@-r15
    mov    #TCB_sp,r1        /* タスクスタックを保存           */
    mov.l  r15,@(r0,r1)    
    mov.l  ret_int_r_k,r1    /* 実行再開番地を保存             */
    mov    #TCB_pc,r2	
    mov.l  r1,@(r0,r2)  
    bra    dispatcher
     nop
    
ret_int_r:
    mov.l  @r15+,r8		
    mov.l  @r15+,r9		
    mov.l  @r15+,r10		
    mov.l  @r15+,r11		
    mov.l  @r15+,r12		
    mov.l  @r15+,r13		
    mov.l  @r15+,r14		
ret_int_1:
    /*
     *  タスク例外処理ルーチンの起動
     *  ret_int_r は dispatcher から呼び出されるため，tcbのアドレスはr0に
     *  入っている．
     *  なお，割込みの出口でタスクスイッチが発生しない場合はディスパッチャ
     *  を通らずに ret_int から直接 ret_int_1 に飛ぶ       
     */
    mov.l  @(TCB_enatex,r0),r1
    mov.l  _enatex_mask_ret,r2
    tst    r1,r2
    bt     ret_int_2            /* enatex が FALSE ならリターン  */
    mov.l  @(TCB_texptn,r0),r1  /* texptnをロード                */
    tst    r1,r1
    bt     ret_int_2            /* texptn が 0で無ければT<= 0    */
    mov.l _call_texrtn_ret,r1   /* タスク例外ルーチンの呼び出し  */
    jsr   @r1        
     nop       
ret_int_2:               
#ifdef SUPPORT_CHG_IPM
    mov    #44,r0
    mov.l  @(r0,r15),r1
    mov.l  _unmask_ipm,r2
    and    r2,r1
    mov.l  _task_intmask_k,r2
    mov.l  @r2,r3
    or     r3,r1
    mov.l  r1,@(r0,r15)
#endif /* SUPPORT_CHG_IMP */
    mov.l  @r15+,r7            /* spc,pr,ssr,スクラッチレジスタを復帰 */
    mov.l  @r15+,r6        
    mov.l  @r15+,r5
    mov.l  @r15+,r4
    mov.l  @r15+,r3
    mov.l  @r15+,r2
    mov.l  @r15+,r1
    mov.l  _mask_md_bl_ret,r0
    ldc    r0,sr               /* BLを1に                               */
    mov.l  @r15+,r0
    lds.l  @r15+,mach                
    lds.l  @r15+,macl        
    ldc.l  @r15+,gbr       
    ldc.l  @r15+,ssr    
    lds.l  @r15+,pr
    ldc.l  @r15+,spc
    rte
     nop
        
    .align 4
_runtsk_ret:
    .long _runtsk
_schedtsk_ret:	
    .long _schedtsk
_enadsp_ret:	
    .long _enadsp
ret_int_r_k:
    .long ret_int_r
_enatex_mask_ret:    
    .long TCB_enatex_mask        
_call_texrtn_ret:	
    .long _call_texrtn
_mask_md_bl_ret:	
    .long  0x50000000

/*
 *  タスク起動処理
 */ 
    .text
    .align 2
    .globl _activate_r
_activate_r:
    stc    sr,r0               /* 割り込み許可 */
    mov.l  _unmask_ipm,r1
    and    r1,r0	
#ifdef SUPPORT_CHG_IPM
    mov.l  _task_intmask_k,r1
    mov.l  @r1,r2
    or     r2,r0
#endif /* SUPPORT_CHG_IPM */
    ldc    r0,sr
    mov.l  @r15+,r1
    lds.l  @r15+,pr
    mov.l  @r15+,r4
    jmp    @r1
     nop
    .align 4
_unmask_ipm:
    .long  0xffffff0f
#ifdef SUPPORT_CHG_IPM		
_task_intmask_k:
    .long _task_intmask
#endif /* SUPPORT_CHG_IPM */
		
		
/*
 *  Exception 600 vector
 *  割り込みが発生すると実行される。		
 *
 *
 *  SH3は割り込みが発生するとすべてVBR+0x600番地からプログラムを実行するため、
 *  ここに配置するルーチンでまずスタックの切り替え、レジスタの保存,IPMの設定,
 *  割り込み要因の判定を行いその後BLビットを0にして割り込みハンドラを呼ぶ必要
 *  がある。
 *  割り込み要因に対応した割り込みハンドラの開始番地は配列int_table[]に登録し
 *  て呼び出す。オフセットの計算は、割り込み要因レジスタを右に2ビットシフトし
 *  て行う。割り込み要因レジスタはSH7708ではINTEVTレジスタにセットされるが、
 *  SH7709およびSH7709AではINTEVT2にセットされるため、ifdefにより切り分けてい
 *  る。また、一部のディバイスについて割り込み要因レジスタで渡される値とIPMと
 *  の間には関係がないため、各割り込み要因に対応したIPMを持つSRの内容を配列
 *  int_plevel_table[]に登録してそれを読み出す。
 *
 *  reqflg をチェックする前に割込みを禁止しないと，reqflg をチェック後
 *  に起動された割込みハンドラ内でディスパッチが要求された場合に，ディ
 *  スパッチされない．
 *
 */

    .org    0x0600        /* _BASE_VBR + 0x0600番地に配置 */
    .align 2
    .global _interrupt
_interrupt:
    stc.l  spc,@-r15       /* 多重割り込みが入ると消えてしまうので        */
    sts.l  pr,@-r15        /* spc,pr,ssr,gbr,macl,mach                    */
    stc.l  ssr,@-r15    
    stc.l  gbr,@-r15                    
    sts.l  macl,@-r15                    
    sts.l  mach,@-r15                                                    
    stc.l  r0_bank,@-r15   /* ,r0〜r7をスタックに保存   */ 
    stc.l  r1_bank,@-r15
    stc.l  r2_bank,@-r15
    stc.l  r3_bank,@-r15
    stc.l  r4_bank,@-r15
    stc.l  r5_bank,@-r15
    stc.l  r6_bank,@-r15    
    stc.l  r7_bank,@-r15
    tst    r7,r7                 /* 割り込み発生時のコンテキストを判定  */  
    bf/s    _interrupt_from_int  /* 例外/割り込みハンドラならジャンプ   */
     add    #0x01,r7             /* 割り込みのネスト回数をインクリメント*/
    mov    r15,r1                /* スタックを入れ替え元のスタックポイ  */
    mov.l  _stacktop_k,r15       /* ンタを保存                          */
    mov.l  r1,@-r15            
    mov.l  _intevt,r0            /* 例外要因レジスタを取得              */
    mov.l  @r0,r4              
    shlr2  r4                    /* 3ビット右シフトしオフセットを求める */
    shlr   r4                  
    mov.l  _int_plevel_table_k,r0 
    mov.l  @(r0,r4),r5           /* 割り込みの優先度を取得               */
    mov.l  _int_table_k,r0    
    mov.l  @(r0,r4),r2           /* 割り込みハンドラのアドレスを取得    */
    ldc    r2,r2_bank            /* バンク0に切り替えるためコピー       */    
    ldc    r5,sr                 /* これ以降割り込みを受け付ける。      */
    jsr    @r2                   /* 割り込みハンドラへ                  */
     nop
    mov.l  _mask_md_ipm_ret,r0
    ldc    r0,sr                 /* 割り込み禁止                           */
    stc    r7_bank,r7            /* 例外/割り込みのネスト回数をデクリメント*/
    dt     r7
    ldc    r7,r7_bank
    mov.l  _reqflg_k,r4          /* reqflgのチェック                       */
    mov.l  @r4,r1
    tst    r1,r1                 /* reqflgがFALSEならret_to_task_intに飛ぶ */
    bt/s   _ret_to_task_int      /* タスクに戻る                           */
     mov.l  @r15,r15             /* 戻り先がタスクなのでスタックを戻す     */
    xor    r0,r0
    bra    ret_int
     mov.l  r0,@r4               /* reqflgをクリア                   */
_interrupt_from_int:
    mov.l   _intevt,r0           /* 例外要因レジスタを取得           */
    mov.l   @r0,r4
    shlr2   r4                   /* オフセットを求める               */
    shlr    r4                   /* オフセットを求める               */    
    mov.l   _int_plevel_table_k,r0 
    mov.l   @(r0,r4),r5            /* 割り込みの優先度を取得           */
    mov.l   _int_table_k,r0        
    mov.l   @(r0,r4),r2            /* 割り込みハンドラのアドレスを取得 */
    ldc     r2,r2_bank             /* バンク0に切り替えるためコピー    */
    ldc     r5,sr                  /* これ以降割り込みを受け付ける。   */
    jsr     @r2                    /* 割り込みハンドラへ               */ 
     nop
    mov.l  _mask_md_ipm_ret,r0
    ldc    r0,sr                   /* 割り込み禁止                     */
    stc    r7_bank,r7              /* 例外/割り込みのネスト回数をデクリメント*/
    dt     r7
    ldc    r7,r7_bank
_ret_to_task_int:
    mov.l  @r15+,r7                /* 割り込み元に戻る                 */
    mov.l  @r15+,r6
    mov.l  @r15+,r5
    mov.l  @r15+,r4
    mov.l  @r15+,r3
    mov.l  @r15+,r2
    mov.l  @r15+,r1
    mov.l  _mask_md_bl_int,r0 
    ldc    r0,sr                   /* BL を1に                         */
    mov.l  @r15+,r0
    lds.l  @r15+,mach                
    lds.l  @r15+,macl        
    ldc.l  @r15+,gbr                       
    ldc.l  @r15+,ssr
    lds.l  @r15+,pr
    ldc.l  @r15+,spc
    rte
     nop

    /*
     * 未登録の割込みが発生すると，その時の INTEVT,INTEVT2,SR,PC を
     * 引数にcpu_interrupt()を呼び出す．
     */   
    .global _no_reg_interrupt
_no_reg_interrupt:
    mov.l _intevt1,r0
    mov.l @r0,r4
#if defined(SH7709) || defined(SH7709A) || defined(SH7729R) || defined(SH7727) 
    mov.l _intevt2,r0    
    mov.l @r0,r5
#endif 
    stc   spc,r6
    stc   ssr,r7
    mov.l _cpu_interrupt_k,r1
    jsr   @r1
     nop

        
    .align  4
_stacktop_k:    
    .long  STACKTOP            /* 非タスク独立部のスタックの初期値  */
_intevt:        
#if defined(SH7708) || defined(SH7750)    
    .long  INTEVT
#else
    .long  INTEVT2
#endif    
_int_table_k:    
    .long    _int_table       
_int_plevel_table_k:    
    .long    _int_plevel_table 
_reqflg_k:
    .long  _reqflg
_mask_md_ipm_ret:    
    .long  0x40000000 + MAX_IPM << 4
_mask_md_bl_int:    
    .long  0x50000000
_intevt1:        
    .long  INTEVT
#if defined(SH7709) || defined(SH7709A) || defined(SH7729R) || defined(SH7727) 
_intevt2:        
    .long  INTEVT2
#endif 
_cpu_interrupt_k:
    .long  _cpu_interrupt   

    
/*
 *  微少時間待ち
 */
	.globl _sil_dly_nse
_sil_dly_nse:
    mov.l  _sil_dly_tim1, r1
    add    r1, r4
    cmp/pl r4
    bt     _sil_dly_nse1
    rts
     nop
_sil_dly_nse1:            
    mov.l  _sil_dly_tim2, r1
    add    r1, r4
    cmp/pl r4
    bt     _sil_dly_nse1
    rts
     nop   
    .align 4
_sil_dly_tim1:
    .long  -SIL_DLY_TIM1    
_sil_dly_tim2:                         
    .long  -SIL_DLY_TIM2
