/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cpu_rename.h,v 1.2 2003/06/30 17:22:47 takayuki Exp $
 */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#ifndef OMIT_RENAME

#define _reset			_kernel__reset
#define vector_entry_SBI	_kernel_vector_entry_SBI
#define vector_entry_RIE	_kernel_vector_entry_RIE
#define vector_entry_AE		_kernel_vector_entry_AE
#define vector_entry_trap	_kernel_vector_entry_trap
#define _trap_handler_0		_kernel__trap_handler_0
#define _trap_handler_1		_kernel__trap_handler_1
#define _trap_handler_2		_kernel__trap_handler_2
#define _trap_handler_3		_kernel__trap_handler_3
#define _trap_handler_4		_kernel__trap_handler_4
#define _trap_handler_5		_kernel__trap_handler_5
#define _trap_handler_6		_kernel__trap_handler_6
#define _trap_handler_7		_kernel__trap_handler_7
#define _trap_handler_8		_kernel__trap_handler_8
#define _trap_handler_9		_kernel__trap_handler_9
#define _trap_handler_10	_kernel__trap_handler_10
#define _trap_handler_11	_kernel__trap_handler_11
#define _trap_handler_12	_kernel__trap_handler_12
#define _trap_handler_13	_kernel__trap_handler_13
#define _trap_handler_14	_kernel__trap_handler_14
#define _trap_handler_15	_kernel__trap_handler_15
#define interrupt_handler	_kernel_interrupt_handler
#define InterruptHandlerEntry	_kernel_InterruptHandlerEntry
#define interrupt_handler_r	_kernel_interrupt_handler_r
#define recover_task_r		_kernel_recover_task_r
#define recover_task		_kernel_recover_task
#define recover_int		_kernel_recover_int
#define exit_interrupt_handler	_kernel_exit_interrupt_handler
#define _exception_handler	_kernel__exception_handler
#define ExceptionHandlerEntry	_kernel_ExceptionHandlerEntry
#define _unhandled_interrupt_handler	_kernel__unhandled_interrupt_handler
#define exit_exception_handler	_kernel_exit_exception_handler
#define exception_handler_r	_kernel_exception_handler_r
#define recover_exception_exc_task_r	_kernel_recover_exception_exc_task_r
#define dispatch		_kernel_dispatch
#define reqflg			_kernel_reqflg
#define calltex			_kernel_calltex
#define runtsk			_kernel_runtsk
#define dispatch_r		_kernel_dispatch_r
#define exit_and_dispatch	_kernel_exit_and_dispatch
#define schedtsk		_kernel_schedtsk
#define activate_r		_kernel_activate_r

#ifdef LABEL_ASM

#define __reset			__kernel__reset
#define _vector_entry_SBI	__kernel_vector_entry_SBI
#define _vector_entry_RIE	__kernel_vector_entry_RIE
#define _vector_entry_AE	__kernel_vector_entry_AE
#define _vector_entry_trap	__kernel_vector_entry_trap
#define __trap_handler_0	__kernel__trap_handler_0
#define __trap_handler_1	__kernel__trap_handler_1
#define __trap_handler_2	__kernel__trap_handler_2
#define __trap_handler_3	__kernel__trap_handler_3
#define __trap_handler_4	__kernel__trap_handler_4
#define __trap_handler_5	__kernel__trap_handler_5
#define __trap_handler_6	__kernel__trap_handler_6
#define __trap_handler_7	__kernel__trap_handler_7
#define __trap_handler_8	__kernel__trap_handler_8
#define __trap_handler_9	__kernel__trap_handler_9
#define __trap_handler_10	__kernel__trap_handler_10
#define __trap_handler_11	__kernel__trap_handler_11
#define __trap_handler_12	__kernel__trap_handler_12
#define __trap_handler_13	__kernel__trap_handler_13
#define __trap_handler_14	__kernel__trap_handler_14
#define __trap_handler_15	__kernel__trap_handler_15
#define _interrupt_handler	__kernel_interrupt_handler
#define _InterruptHandlerEntry	__kernel_InterruptHandlerEntry
#define _interrupt_handler_r	__kernel_interrupt_handler_r
#define _recover_task_r		__kernel_recover_task_r
#define _recover_task		__kernel_recover_task
#define _recover_int		__kernel_recover_int
#define _exit_interrupt_handler	__kernel_exit_interrupt_handler
#define __exception_handler	__kernel__exception_handler
#define _ExceptionHandlerEntry	__kernel_ExceptionHandlerEntry
#define __unhandled_interrupt_handler	__kernel__unhandled_interrupt_handler
#define _exit_exception_handler	__kernel_exit_exception_handler
#define _exception_handler_r	__kernel_exception_handler_r
#define _recover_exception_exc_task_r	__kernel_recover_exception_exc_task_r
#define _dispatch		__kernel_dispatch
#define _reqflg			__kernel_reqflg
#define _calltex		__kernel_calltex
#define _runtsk			__kernel_runtsk
#define _dispatch_r		__kernel_dispatch_r
#define _exit_and_dispatch	__kernel_exit_and_dispatch
#define _schedtsk		__kernel_schedtsk
#define _activate_r		__kernel_activate_r

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_RENAME_H_ */
