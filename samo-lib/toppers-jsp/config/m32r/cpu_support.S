/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006 by Monami Software Limited Partnership, JAPAN	
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
 *  @(#) $Id: cpu_support.S,v 1.12 2007/05/30 03:56:47 honda Exp $
 */

/*
 *	プロセッサ依存モジュール アセンブリ言語部（M32R用）
 */

#define	_MACRO_ONLY

#include <cpu_rename.h>

#include <m32rasm.inc>
#include <s_services.h>

#include "offset.h"

#define _trap_handler_0 _kernel__trap_handler_0
#define _trap_handler_1 _kernel__trap_handler_1
#define _trap_handler_2 _kernel__trap_handler_2
#define _trap_handler_3 _kernel__trap_handler_3
#define _trap_handler_4 _kernel__trap_handler_4
#define _trap_handler_5 _kernel__trap_handler_5
#define _trap_handler_6 _kernel__trap_handler_6
#define _trap_handler_7 _kernel__trap_handler_7
#define _trap_handler_8 _kernel__trap_handler_8
#define _trap_handler_9 _kernel__trap_handler_9
#define _trap_handler_10 _kernel__trap_handler_10
#define _trap_handler_11 _kernel__trap_handler_11
#define _trap_handler_12 _kernel__trap_handler_12
#define _trap_handler_13 _kernel__trap_handler_13
#define _trap_handler_14 _kernel__trap_handler_14
#define _trap_handler_15 _kernel__trap_handler_15

	.extern	_kernel_runtsk
	.extern	_kernel_schedtsk
	.extern	_kernel_reqflg
	.extern _kernel_enaflg
	.extern	InterruptHandlerEntry
	.extern	ExceptionHandlerEntry
	.extern	ext_tsk
		
/*
 *  EITベクタエントリの定義
 */
	.section ".vector","ax"
	.global	  reset
	.extern   start

	.balign 0x10
reset:
	bra		start

	.balign 0x10
vector_entry_SBI:
    /* 
	 *  システムブレーク割込み
	 */
	bra 	vector_entry_SBI

	.balign 0x10
vector_entry_RIE:
    /* 
	 *  予約命令例外（Reserved Instruction Exception）
	 */
	stmdb	"r0,r1"
	ldi		r1, #(EXC_RIE*4-4)
	bra		_exception_handler

	.balign 0x10
vector_entry_AE:
    /* 
	 *  アドレス例外（Address Exception）
	 */
	stmdb	"r0,r1"
	ldi		r1, #(EXC_AE*4-4)
	bra		_exception_handler

	.balign 0x10
vector_entry_trap:
    /*
	 *  トラップベクタエントリ
	 */
	bra		_trap_handler_0
	bra		_trap_handler_1
	bra		_trap_handler_2
	bra		_trap_handler_3
	bra		_trap_handler_4
	bra		_trap_handler_5
	bra		_trap_handler_6
	bra		_trap_handler_7
	bra		_trap_handler_8
	bra		_trap_handler_9
	bra		_trap_handler_10
	bra		_trap_handler_11
	bra		_trap_handler_12
	bra		_trap_handler_13
	bra		_trap_handler_14
	bra		_trap_handler_15

/*
 *  割込みベクタエントリと割込みハンドラ起動／復帰処理部
 *
 *  割込み発生時のハードウエアの動作
 *
 *  pswレジスタ中のsm，ie，pm，cビットをbsm，bie，bpm，bcビットに退避
 *  する．bpcには割込み発生時のpc値が入る．
 *
 *	bsm <- sm
 *  bie <- ie
 *  bpm <- pm
 *  bc <- c
 *  bpc <- 割込み発生時のpc値
 *
 *	pswレジスタ中のsm，ie，pm，cビットを以下のように更新する．
 *
 *	sm <- 0
 *  ie <- 0
 *  pm <- 0
 *  c <- 0
 *
 *　レジスタの積み方
 *  r0-r1（作業領域）
 *  r0-r7，r14，acc(hi/lo)，bpc，psw，ICUISTS，psw（割込みハンドラ呼出前）
 *  
 *  二つ目のpsw以外は，割込み発生時のコンテキストに対応したスタックに
 *  退避する．2つ目のpswは，割込みハンドラの実行直後に復帰するために，
 *  必ず割込みスタックに退避する．遅延ディスパッチする場合には，上記の
 *  レジスタ以外にも，r8-r13を退避してディスパッチャを呼び出す．
 */
	.balign 0x10
interrupt_handler:

	/* 
	 *  作業用レジスタの退避 
	 *
	 *  最初に作業用レジスタをスタックに退避する．割込み発生直後は，必
	 *  ず割込みスタックになるので，ここで退避する作業用レジスタは割込
	 *  みスタックに積まれる．
	 */
	stmdb	"r0,r1"

	/* 
	 *  割込み発生時のコンテキスト判定
	 *
	 *  pswのbsmビットから割込み発生時に使用していたスタック情報を取得
	 *  する．bsmが0の場合は，割込みスタックを使用していたことになるの
	 *  で，割込みコンテキストで割込みが発生したと判断する．bsmが1の場
	 *  合は，ユーザスタックを使用していたことになるので，タスクコンテ
	 *  キストで割込みが発生したと判断する．後者の場合は，作業領域（r0
	 *  とr1）の積み直しは必要ない．
	 */
	mvfc	r0, psw
	and3	r1, r0, 0x8000
	beqz	r1, L1

	/* 
	 *  退避レジスタのユーザスタックへの積み直し  
	 *
	 *  タスクコンテキストで割込みが発生した場合は，割込みスタックに退
	 *  避した作業用レジスタr0とr1をユーザスタックに積み直す必要がある．
	 */

	or3		r0, r0, 0x80		
	mvtc	r0, psw				/* ユーザスタックに切替（sp = spu） */

	mvfc	r1, spi
	ld		r0, @(4, r1)		/* 退避したr0を割込みスタックから取り出す */
	st		r0, @-sp			/* r0をユーザスタックに積む */
	ld		r0, @r1				/* 退避したr1を割込みスタックから取り出す */
	st		r0, @-sp			/* r1をユーザスタックに積む */
	addi	r1, 8				/* 割込みスタックを作業スタック退避前に戻す */
	mvtc	r1, spi				/* 割込みスタック位置をspiに保存 */
	mvfc	r0, psw

L1:
	/* 
	 *  コンテキストの退避
	 * 
	 *  r2-r7, r14, acch, accl, bpc, psw, 割込みマスク（ICUISTS）,psw
	 *  の順にスタックに退避する．ここでは，必ずpswがr0に保存されてい
	 *  る．
	 */
	stmdb	"r2,r3,r4,r5,r6,r7,r14"
	mvfachi	r1
	st		r1, @-sp			/* acchを退避 */
	mvfaclo	r1
	st		r1, @-sp			/* acclを退避 */
	mvfc	r1, bpc
	st		r1, @-sp			/* bpcを退避 */
	st 		r0, @-sp			/* pswを退避 */

	ld24	r1, ICUISTS
	ld		r0, @r1				/* 割込みマスク読み込み（割込み要求はクリアされる）*/
	st		r0, @-sp			/* 割込みマスクを退避 */

	lea		r1, InterruptHandlerEntry /* 割込みハンドラテーブルの先頭アドレスを取り出す */
	srli	r0, 22				/* 割込み要因番号を取得 */
	addi	r0, -1				/* 割込みハンドラテーブルのインデックスを計算 */
	slli	r0, 2				/* 割込みハンドラテーブルのオフセットを計算 */
	add		r1, r0				/* 割込みハンドラテーブル中の割込みハンドラのアドレス計算 */
	ld		r1, @r1				/* 割込みハンドラの起動番地を取り出す */

L2:
	/*
	 *  割込みハンドラの呼出し
	 *
	 *  pswを退避し，割込みスタック, 割込み許可にして割込みハンドラを
	 *  呼び出す．割込みハンドラからの戻り番地をeit_handler_rとする．
	 *  ここで，再度pswを退避するのは，eit_handler_rの最初で，使用する
	 *  スタックを戻す必要があるためで，必ず割込みスタックに退避してお
	 *  く．
	 */
	mvfc	r2, psw						
	and3	r3, r2, 0xff7f				
	mvtc	r3, psw						/* 割込みスタック */
	st		r2, @-sp					/* pswを割込みスタックに退避 */
	lea		r14, eit_handler_r 			/* 戻り番地を設定 */
	or3		r3, r3, 0x40				
	mvtc	r3, psw						/* 割込み許可 */
	jmp		r1							/* ハンドラ起動 */

	.text
	.align 4
eit_handler_r:
	/*
	 *  EITハンドラからの復帰処理
	 *
	 *  EITハンドラの実行が終了すると呼び出される．割込み発生時のコン
	 *  テキスト，割込み要求の有無，ディスパッチ許可／禁止状態から，適
	 *  切な復帰処理を行なう．
	 */
	ld		r0, @sp+
	mvtc	r0, psw			/* 割込みスタックからpswを復帰（EIT発生時
							   のコンテキストに戻る．割込みは禁止状態）
							    */

	ld24 	r1, ICUIMASK
	ld 		r2, @sp+
	st 		r2, @r1			/* 割込みマスクの復帰 */

	and3	r0, r0, 0x8000
	beqz	r0, recover_int	/* EIT発生時のコンテキストが割込みコンテキ
							   ストであれば，多重割込みであると判断し，
							   即座に共通復帰処理へ */

	/* 
	 *  タスクコンテキストへの復帰ルーチン
	 *
	 *  ディスパッチ／タスク例外起動要求がない場合は，即座に共通復帰ルー
	 *  チンに飛ぶ．遅延ディスパッチ処理を行なう．ディスパッチ要求がな
	 *  ければ共通の復帰処理（recover_int）を行なう．
	 */
	lea		r0, _kernel_reqflg
	ld		r1, @r0
	beqz	r1, recover_int		/* 共通復帰ルーチンへ */

	/* 
	 *  ディスパッチ／タスク例外起動要求がある場合の処理
	 *
	 *  まず，ディスパッチ要求フラグ（reqflg）をクリアする．そして，ディ
	 *  スパッチ許可フラグ（enadsp）がFALSEの場合と，runtskとschedtsk 
	 *  が同じ場合には，ディスパッチせずに，タスク例外起動処理ルーチン
	 *  へ飛ぶ．ディスパッチが許可されており，runtskとschedtskが異なる
	 *  場合には，遅延ディスパッチ処理を行う．
	 */
	ldi		r1, #0
	st		r1, @r0				/* reqflgをクリア */

	lea 	r0,  _kernel_enadsp
	ld 		r0, @r0
	beqz 	r0, recover_task 	/* enaflgのチェック */

	lea		r0, _kernel_runtsk
	lea		r1, _kernel_schedtsk
	ld		r0, @r0	
    ld		r1, @r1
    beq		r0, r1, recover_task	/* runtskとschedtskが同じ場合はタ
								       スク例外起動処理へ */

	/*
	 *  例外／割込み出口での遅延ディスパッチ処理
	 *
	 *  退避していない残りのレジスタを退避し，実行状態タスクのpcとspを
	 *  退避してからディスパッチャを呼び出す．
	 */
	stmdb	"r8,r9,r10,r11,r12,r13"

	lea		r0, _kernel_runtsk
	ld		r0, @r0

	lea		r1, recover_task_r
	st		r1, @(TCB_pc, r0)
	st		sp, @(TCB_sp, r0)
	bra		_kernel_exit_and_dispatch

	.align 4
recover_task_r:
	ldmia	"r13,r12,r11,r10,r9,r8"

recover_task:
	/* 
	 *  タスク例外起動処理
	 *
	 *  ここで，enatexがTRUEで，texptnが0でなければ，タスク例外処理ルー
	 *  チンを呼び出す．calltexの処理内容（runtsk->enatexがTRUEで，
	 *  runtsk->texptnが0でない場合に，call_texrtnを呼ぶ）をここにイン
	 *  ライン展開した方が効率が良いが，実装の簡易化のためにcalltex を
	 *  呼び出す．
	 */
	bl		_kernel_calltex

recover_int:
	/* 
	 *  EIT共通復帰処理（低優先度割込みへの復帰含む）
	 *
	 *  退避した順番（r2-r7, r14, acch, accl, bpc, psw）の逆順でスタッ
	 *  クから取り出して復帰する．また，作業用レジスタ（r0，r1）も復帰
	 *  する．
	 */
	ld 		r0, @sp+
	mvtc 	r0, psw						/* pswを復帰 */
	ld		r0, @sp+			
	mvtc	r0, bpc						/* bpcを復帰 */
	ld		r0, @sp+	
	mvtaclo	r0							/* acclを復帰 */
	ld		r0, @sp+
	mvtachi	r0							/* acchlを復帰 */
	ldmia	"r14,r7,r6,r5,r4,r3,r2"		/* その他のレジスタを復帰 */
	ldmia	"r1,r0"						/* 作業用レジスタの復帰 */
	rte									/* 割込み処理から復帰 */

	.section ".vector","ax"
	/*
	 *  トラップのエントリ部 : トラップ番号の格納
	 */
	.irp num,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
		.align	4

	_kernel__trap_handler_\num:
		stmdb	"r0,r1"
		ldi		r1, #((\num + EXC_TRAP00 - 1) * 4)
		bra		_exception_handler
	.endr
		
/*
 *  例外／トラップハンドラ入口処理
 *
 *  例外が発生した場合には，スクラッチレジスタを退避して例外要因（r1に
 *  入っている）に対応する例外ハンドラを呼び出す．例外ハンドラからの復
 *  帰処理は，割込みハンドラからの復帰処理と共通（eit_handler_r）である．
 *
 *  例外発生時のハードウエアの動作
 *
 *  pswレジスタ中のsm，ie，pm，cビットをbsm，bie，bpm，bcビットに退避
 *  する．bpcには例外を起こした命令のpc値が入る．
 *
 *	bsm <- sm
 *  bie <- ie
 *  bpm <- pm
 *  bc <- c
 *  bpc <- 例外を起こした命令のアドレス
 *
 *	pswレジスタ中のsm，ie，pm，cビットを以下のように更新する．
 *
 *	sm <- 不変
 *  ie <- 0
 *  pm <- 0
 *  c <- 0
 *
 *  レジスタの積み方
 *
 *  r0-r1（作業領域，ベクタの最初で退避）
 *  r2-r7，r14，acc(hi/lo)，bpc，psw，ICUIMASK（例外ハンドラ呼出前）
 *
 *  ここで，ICUISTSではなくICUIMASKを退避するのは，例外処理中に外部割
 *  込みが発生した場合に，ICUISTSの読込みにより割込み要因がクリアされ
 *  てしまうのを防ぐためである．
 */
_exception_handler:

	stmdb 	"r2,r3,r4,r5,r6,r7,r14"
	mvfachi r0
	st 		r0, @-sp
	mvfaclo r0
	st 		r0, @-sp
	mvfc 	r0, bpc
	st 		r0, @-sp
	mvfc 	r2, psw
	st 		r2, @-sp
	ld24 	r0, ICUIMASK
	ld 		r0, @r0
	st 		r0, @-sp

	lea		r0, ExceptionHandlerEntry
	add		r0, r1
	ld		r1, @r0				/* 例外ハンドラのアドレスを取得 */

	mv 		r0, sp				/* スタックの位置を例外ハンドラの引数
								   に設定 */
	and3 	r3, r2, 0x7f00
	srli 	r3, 8
	mvtc 	r3, psw 			/* 割込みスタック，割込み許可状態に */
	st 		r2, @-sp			/* 例外発生直後のpswを割込みスタックに退避 */
	lea 	r14, eit_handler_r 
	jmp 	r1
				
Function _kernel_dispatch
	/*
	 *  タスク切替えのための退避処理
	 *
	 *  このルーチンは，タスクコンテキスト，CPUロック状態，ディスパッ
	 *  チ許可状態で呼び出される．呼出元では保存されないレジスタをスタッ
	 *  クに退避し，pcとspはTCBに退避する．pcはディスパッチャ出口処理
	 *  （dispatch_r）に設定する．
	 */
	stmdb	"r8,r9,r10,r11,r12,r13,r14"

	lea 	r0, _kernel_runtsk
	lea		r1, dispatch_r
	ld		r0, @r0
	st		r1, @(TCB_pc, r0)		/* 復帰する時はdispatch_rに帰ってくる */
	st		sp, @(TCB_sp, r0)		/* スタックポインタを退避 */

Function _kernel_exit_and_dispatch
	/*
	 *  タスク切替えの前処理
	 *
	 *  schedtskが空でないかをチェックする．schedtskが空でない場合は，
	 *  タスク切替え処理に移る．空の場合は，割込みを許可してループする
	 *  処理に移る．
	 */	
	xor		r0, r0
	mvtc	r0, psw						/* 割込み禁止，割込みスタック */
	
	lea		r1,  _kernel_schedtsk
L6:	ld		r0, @r1
	bnez	r0, L7						/* schedtskの有無をチェック */

	/* 
	 *  起動すべきタスクがない場合の処理
	 *
	 *  起動すべきタスクがない場合は，割込みを許可してループする．ルー
	 *  プの途中で割込みを許可し，実行可能タスクを待つ．
	 */
	ldi		r0, 0x40
	mvtc	r0, psw						/* 割込み許可 */
	xor		r0, r0
	mvtc	r0, psw						/* 割込み禁止 */

	lea 	r0, _kernel_reqflg
	xor 	r2, r2
	st 		r2, @r0 					/* reqflg = 0 */
	bl 		L6

L7:
    /* 
	 *  タスク切替え処理
	 *
	 *  実行中タスク（runtsk）に最高優先順位タスク（schedtsk）を設定し，
	 *  スタックをユーザスタックに切替える．runtskのTCBのspとpcを復帰
	 *  する．ここでは，r0 = schedtskである．
	 */
	lea		r2, _kernel_runtsk
	st		r0, @r2 					/* runtsk = schedtsk */

	ldi 	r1, 0x80
	mvtc 	r1, psw 					/* ユーザスタック，割込み禁止 */

	ld		sp, @(TCB_sp, r0)			/* スタックポインタを復帰 */
	ld		r0, @(TCB_pc, r0)			/* プログラムカウンタを復帰 */
	jmp		r0

	.align 4
Label dispatch_r
	/* 
	 *  ディスパッチャの出口処理
	 * 
	 *  dispatchで退避したレジスタを復帰し，タスク例外処理ルーチンを呼
	 *  び出す．
	 */
	bl 		_kernel_calltex 
    ldmia 	"r14,r13,r12,r11,r10,r9,r8" 
	rts

Function _kernel_activate_r
	/*
	 *  タスクの起動ルーチン
	 *
	 *  runtskのタスク初期化ブロックからタスク関数と拡張情報を取り出し
	 *  て，拡張情報を引数レジスタ（r0）に設定する．ext_tskを呼び出さ
	 *  ずにタスクが終了する場合があるため，タスク関数の戻り番地を
	 *  ext_tskに設定する．起動準備が全て完了してから，割込みを許可し
	 *  てタスク関数に飛ぶ．
	 */
	lea		r2, _kernel_runtsk
	ld		r2, @r2
	ld		r2, @(TCB_tinib, r2)		/* r2 = runtsk->tinib */
	ld		r0, @(TINIB_exinf, r2)
	ld		r1, @(TINIB_task,  r2)

	lea		r14, ext_tsk				/* タスクの戻り番地をext_tskに */

	ldi 	r2, 0xc0
	mvtc 	r2, psw						/* ユーザスタック，割込み許可 */
	jmp		r1							/* タスク関数へ */
