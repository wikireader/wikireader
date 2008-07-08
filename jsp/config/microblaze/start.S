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
 *  @(#) $Id: start.S,v 1.9 2004/02/05 09:16:25 honda Exp $
 */

#define _MACRO_ONLY
#include <microblaze_asm.inc>
#include <t_config.h>
#include <sys_defs.h>
/*
 *  # 0x00 #		Imm instr for reset address     [Hi halfword]
 *  # 0x04 #		Jump to _start                  [Lo halfword]
 *  # 0x08 #		Imm instr for exception address [Hi halfword]
 *  # 0x0c #		Jump to Exception handler       [Lo halfword]
 *  # 0x10 #		Imm instr for interrupt address [Hi halfword]
 *  # 0x14 #		Jump to interrupt handler       [Lo halfword]
 */

        
	.text
	.global _start
	.align  2
    .ent _start
_start:
    /*
     *  msrを初期化
     */
    la   r1, r0, MSR_CACHE_SETTING         
    mts  rmsr,r1 
                
    /*
     *  opb_mdm を使うと msr のBIPビットが'1'で実行されるため，
     *  rtbd命令により'0'に初期化する．
     *  stub を使う場合は必要ないが特に副作用がないため，無条件に実行する．
     */
    la    r1, r0, _start_
    rtbd  r1,0
_start_:        
               
    /*
     * R2とR3の設定
     *・R2  : Read-only small data area anchor
     *   _SDA2_BASE_ .sdata2の中心
     *・R13 : Read-write small data area anchor
     *   .sdataとsbssを連続して置き，この中間に_SDA_BASE_ を設定する
     */
	la	r13, r0, _SDA_BASE_
	la	r2,  r0, _SDA2_BASE_

	/*
	 * スタックポインタの初期化
	 */
	la	r1, r0, STACKTOP - 4

   /*
	* ベクタテーブルの初期化
	*/
	/*
     *  brai 命令の書き込み
     */
	la  r3,  r0,  0xb8080000  /* brai のオペコード      */
	swi r3,  r0,  0xc         /* exceptionベクタへ登録  */
	swi r3,  r0,  0x14        /* interrruptベクタへ登録 */
	/*
     * imm 命令の書き込み
     */
	la  r4,  r0,  0xb0000000  /* imm のオペコード       */
	swi r4,  r0,  0x8         /* exceptionベクタへ登録  */
    swi r4,  r0,  0x10        /* interrruptベクタへ登録 */

	/*
     *  例外ハンドラの登録
     */
	la   r5,  r0,  exception_entry
	sw	 r5,  r1,  r0
	lhu  r6,  r1,  r0
	shi	 r5,  r0,  0xe       /* 割込みハンドラ下位16bit */
	shi	 r6,  r0,  0xa       /* 割込みハンドラ上位16bit */

	/*
	 *  割込みハンドラの登録
	 */
	la   r5,  r0,  interrupt_entry
	sw	 r5,  r1,  r0
	lhu  r6,  r1,  r0
	shi	 r5,  r0,  0x16      /* 割込みハンドラ下位16bit */
	shi	 r6,  r0,  0x12      /* 割込みハンドラ上位16bit */


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
	la    r3,  r0,  _hardware_init_hook
	beqi  r3,  start_1
	brald r15, r3
	nop


	/*
     *  bssセクションをクリア
     */
start_1:
	la    r3,  r0,  __bss_start
	la    r4,  r0,  _end
	sub   r5,  r4,  r3
	beqi  r5,  start_3
start_2:
	sb    r0,  r0,  r3
	addi  r3,  r3,  1
	sub   r5,  r4,  r3
	bgti  r5,  start_2

	/*
	 *  dataセクションの初期化（ROM化対応）
	 *
	 *  __idata_start から __idata_end までを，__data_start 以降に
	 *  コピーする．
     *  Microblazeは内臓RAMが小さいことと，BRAMの内容は
     *  リコンフィギュレーションすると初期化されるため
     *  dataセクションの初期化は行わない
     *  ただし場合によっては実装することを考えてラベルのみ定義しておく
	 */
start_3:
	la    r3,  r0,  __idata_start
	la    r4,  r0,  __idata_end
	sub   r5,  r4,  r3     /* サイズをチェック */
	beqi  r5,  start_5
	la    r6,  r0,  __data_start
start_4:
	lbu   r7,  r0,  r3
    sb    r7,  r0,  r6
	addi  r3,  r3,  1
	addi  r6,  r6,  1
	sub   r5,  r4,  r3
	bgti  r5,  start_4


start_5:
	la    r3,  r0,  __isdata2_start
	la    r4,  r0,  __isdata2_end
	sub   r5,  r4,  r3     /* サイズをチェック */
	beqi  r5,  start_7
	la    r6,  r0,  __sdata2_start
start_6:
	lbu   r7,  r0,  r3
    sb    r7,  r0,  r6
	addi  r3,  r3,  1
	addi  r6,  r6,  1
	sub   r5,  r4,  r3
	bgti  r5,  start_6


start_7:
	la    r3,  r0,  __isdata_start
	la    r4,  r0,  __isdata_end
	sub   r5,  r4,  r3     /* サイズをチェック */
	beqi  r5,  start_9
	la    r6,  r0,  __sdata_start
start_8:
	lbu   r7,  r0,  r3
    sb    r7,  r0,  r6
	addi  r3,  r3,  1
	addi  r6,  r6,  1
	sub   r5,  r4,  r3
	bgti  r5,  start_8


	/*
	 *  _software_init_hook を呼び出す（0 でない場合）．
	 *
	 *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
	 *  理がある場合は，software_init_hook という関数を用意すれば
	 *  よい．
	 */
start_9:
	la    r3,  r0,  _software_init_hook
	beqi  r3,  start_10
	brald r15, r3
	nop

start_10:
	brlid	r15, kernel_start
    addik r1,r1,4            /* スタックを元に戻す */
