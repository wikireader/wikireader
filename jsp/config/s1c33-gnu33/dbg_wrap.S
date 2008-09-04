; 
; TOPPERS/JSP Kernel
;     Toyohashi Open Platform for Embedded Real-Time Systems/
;     Just Standard Profile Kernel
;
; Additional routine
;
; Copyright (C) 2003 by SEIKO EPSON Corp, JAPAN
;
;  上記著作権者は，以下の (1)〜(4) の条件か，Free Software Foundation
;  によって公表されている GNU General Public License の Version 2 に記
;  述されている条件を満たす場合に限り，本ソフトウェア（本ソフトウェア
;  を改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
;  利用と呼ぶ）することを無償で許諾する．
;  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
;      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
;      スコード中に含まれていること．
;  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
;      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
;      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
;      の無保証規定を掲載すること．
;  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
;      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
;      と．
;    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
;        作権表示，この利用条件および下記の無保証規定を掲載すること．
;    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
;        報告すること．
;  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
;     害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
;
;  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
;  よびTOPPERSプロジェクトは，本ソフトウェアに関して，その適用可能性も
;  含めて，いかなる保証も行わない．また，本ソフトウェアの利用により直
;  接的または間接的に生じたいかなる損害に関しても，その責任を負わない．
;

#define TPS_STACK_SIZE	0x0c00		; sp is in end of RAM

	.section	.bss		; Stack area for debug command
	.align	2
	.zero	TPS_STACK_SIZE
dbg_stack:

;
; レジスタ退避処理
; Registers saving process
;
.macro	dbgPreWrap
	ld.w	%r5,%sp			; スタックポインタを退避
					; Save current stack pointer.
	xld.w	%r4,dbg_stack		; デバッグ機能用スタック領域を設定:
	ld.w	%sp,%r4			; Set new stack area for debugging.
	ld.w	%r4,%r15		; 復帰先アドレスを退避
					; Save return address.
	pushn	%r9			; レジスタを退避
					; Save registers.
	ld.w    %r6,%r12		; パラメータをコピー
	ld.w    %r7,%r13		; Copy parameters
	ld.w    %r8,%r14
	xld.w	%r15,__dp		; データエリアポインタを設定
.endm					; set data area pointer

;
; レジスタ復元処理
; Registers restoring process
;
.macro	dbgPostWrap
	ld.w    %r10,%r4		; 戻り値をコピー
	ld.w    %r11,%r5		; Copy return value.
	popn	%r9			; レジスタを復元
					; restore registers.
	ld.w	%sp,%r5			; スタックポインタを復元
					; Restore Stack pointer.
	ld.w    %r15,%r4
	jp	%r15			; デバッグ機能を終了:
.endm					; Back to mini monitor

;
; デバッグ機能のラップ処理
; Wrappers for debug functions.
;
	.text
	.global mng
mng:
	dbgPreWrap
	xcall	iprint_mng	; enter C program
	dbgPostWrap


	.global rdy
rdy:
	dbgPreWrap
	xcall	iprint_rdy	; enter C program
	dbgPostWrap


	.global tsk
tsk:
	dbgPreWrap
	xcall	iprint_tsk	; enter C program
	dbgPostWrap


	.global cyc
cyc:
	dbgPreWrap
	xcall	iprint_cyc	; enter C program
	dbgPostWrap


	.global mbx
mbx:
	dbgPreWrap
	xcall	iprint_mbx	; enter C program
	dbgPostWrap


	.global sem
sem:
	dbgPreWrap
	xcall	iprint_sem	; enter C program
	dbgPostWrap


	.global flg
flg:
	dbgPreWrap
	xcall	iprint_flg	; enter C program
	dbgPostWrap


	.global dtq
dtq:
	dbgPreWrap
	xcall	iprint_dtq	; enter C program
	dbgPostWrap


	.global mpf
mpf:
	dbgPreWrap
	xcall	iprint_mpf	; enter C program
	dbgPostWrap

