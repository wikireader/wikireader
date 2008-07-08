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
 *  @(#) $Id: cpu_unrename.h,v 1.6 2007/05/30 03:56:47 honda Exp $
 */

#ifdef _CPU_RENAME_H_
#undef _CPU_RENAME_H_

#ifndef OMIT_RENAME

#undef _reset
#undef vector_entry_SBI
#undef vector_entry_RIE
#undef vector_entry_AE
#undef vector_entry_trap
#undef _trap_handler_0
#undef _trap_handler_1
#undef _trap_handler_2
#undef _trap_handler_3
#undef _trap_handler_4
#undef _trap_handler_5
#undef _trap_handler_6
#undef _trap_handler_7
#undef _trap_handler_8
#undef _trap_handler_9
#undef _trap_handler_10
#undef _trap_handler_11
#undef _trap_handler_12
#undef _trap_handler_13
#undef _trap_handler_14
#undef _trap_handler_15
#undef interrupt_handler
#undef InterruptHandlerEntry
#undef interrupt_handler_r
#undef recover_task_r
#undef recover_task
#undef recover_int
#undef exit_interrupt_handler
#undef _exception_handler
#undef ExceptionHandlerEntry
#undef _unhandled_interrupt_handler
#undef exit_exception_handler
#undef exception_handler_r
#undef recover_exception_exc_task_r
#undef dispatch
#undef reqflg
#undef calltex
#undef runtsk
#undef dispatch_r
#undef exit_and_dispatch
#undef schedtsk
#undef activate_r

#ifdef LABEL_ASM

#undef __reset
#undef _vector_entry_SBI
#undef _vector_entry_RIE
#undef _vector_entry_AE
#undef _vector_entry_trap
#undef __trap_handler_0
#undef __trap_handler_1
#undef __trap_handler_2
#undef __trap_handler_3
#undef __trap_handler_4
#undef __trap_handler_5
#undef __trap_handler_6
#undef __trap_handler_7
#undef __trap_handler_8
#undef __trap_handler_9
#undef __trap_handler_10
#undef __trap_handler_11
#undef __trap_handler_12
#undef __trap_handler_13
#undef __trap_handler_14
#undef __trap_handler_15
#undef _interrupt_handler
#undef _InterruptHandlerEntry
#undef _interrupt_handler_r
#undef _recover_task_r
#undef _recover_task
#undef _recover_int
#undef _exit_interrupt_handler
#undef __exception_handler
#undef _ExceptionHandlerEntry
#undef __unhandled_interrupt_handler
#undef _exit_exception_handler
#undef _exception_handler_r
#undef _recover_exception_exc_task_r
#undef _dispatch
#undef _reqflg
#undef _calltex
#undef _runtsk
#undef _dispatch_r
#undef _exit_and_dispatch
#undef _schedtsk
#undef _activate_r

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_RENAME_H_ */
