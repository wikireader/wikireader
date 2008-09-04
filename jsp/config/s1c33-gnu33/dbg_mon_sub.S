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

#define	_MACRO_ONLY
#include "dbg_mon.h"

;
; Specifies context after timer handler.
;   Return Value : none
;   Parameters   : R6 ... Context type (Output parameter)
;                  R7 ... Control box or initial state information
;                         table        (Output parameter)
;   Function     : This function specifies the context which return
;                  from handler.
;                  this function is specialized for tpsIntPostWrap().
;		   So it presupposes following orders.
;		    1. CPU is locked.
;                   2. This function is called just before recovering
;                      General registers.
;
	.text
	.global tpsSenseRetContext
tpsSenseRetContext:
	xld.w	%r3, _kernel_tps_IntNestCnt
	ld.w	%r4, [%r3]
	cmp	%r4, 0x00
	jreq	tpsSRC_task				; Interrupt in Task/Idle routine

tpsSRC_cyc:
	sub	%r4, 0x01
	xld.w	%r5, _kernel_tps_SymStack
	xld.w	%r6, _kernel_tps_CycHdrTorch
	sll	%r4, 0x02
	ld.w	%r7, [%r6]
	cmp	%r7, 0x00
	jreq.d	tpsSRC_hdr1				; Cyclic handler startFlag off
	add	%r5, %r4
	ld.w	%r8, [%r5]
	xld.w	%r5, TPS_C_TIMER_HANDLER
	cmp	%r8, %r5
	jrne	tpsSRC_hdr2
	jp.d	tpsSRC_exit
	ld.w	%r6, TPS_CTXTYPE_CYCHDR

tpsSRC_hdr1:						; return to other handler
	ld.w	%r7, [%r5]
tpsSRC_hdr2:
	jp.d	tpsSRC_exit
	ld.w	%r6, TPS_CTXTYPE_INTHDR

tpsSRC_task:
	xld.w	%r4, _kernel_runtsk
	ld.w	%r7, [%r4]
	cmp	%r7, 0x00
	jreq	tpsSRC_idle				; return to idle routine
	jp.d	tpsSRC_exit
	ld.w	%r6, TPS_CTXTYPE_TASK

tpsSRC_idle:
	xld.w	%r6, TPS_CTXTYPE_IDLE

tpsSRC_exit:
	ret

