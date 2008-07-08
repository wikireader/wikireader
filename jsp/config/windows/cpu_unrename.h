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
 *  @(#) $Id: cpu_unrename.h,v 1.4 2005/07/06 03:10:29 honda Exp $
 */

/* This file is generated from cpu_rename.def by genrename. */

#ifdef _CPU_UNRENAME_H_
#undef _CPU_UNRENAME_H_

#ifndef OMIT_RENAME

#undef activate_r
#undef ret_int
#undef ret_exc
#undef NotifyDialogProc
#undef InitializeComSupportModule
#undef FinalizeComSupportModule
#undef idispatch
#undef FatalAssertion
#undef CurrentInterruptLevel
#undef InterruptLevel
#undef ExceptionLevel
#undef AnotherExceptionFilter
#undef SystemMutex
#undef SystemMutexOwnerThreadID
#undef SystemMutexLastOwnerThreadID
#undef enter_system_critical_section
#undef leave_system_critiacl_section
#undef wait_for_thread_suspension_completion
#undef isns_int
#undef InterruptHandlerWrapper
#undef def_int
#undef ini_int
#undef fin_int
#undef ras_int
#undef sns_int
#undef sch_int
#undef LockerThreadID
#undef HALExceptionHandler
#undef ini_exc
#undef fin_exc
#undef DebugOutDialogProc
#undef InitializeDebugServices
#undef FinalizeDebugServices
#undef decode_waitstatus
#undef decode_taskstatus
#undef ProcessInstance
#undef PrimaryThreadHandle
#undef PrimaryDialogHandle
#undef CurrentRunningThreadHandle
#undef ShutdownPostponementRequest
#undef WorkerThreadHandle
#undef DestructionProcedureQueue
#undef KernelStarter
#undef TaskExceptionPerformer
#undef ForceShutdownHandler
#undef PrimaryDialogCommandHandler
#undef HALMessageHandler
#undef PrimaryDialogProc

#ifdef LABEL_ASM

#undef _activate_r
#undef _ret_int
#undef _ret_exc
#undef _NotifyDialogProc
#undef _InitializeComSupportModule
#undef _FinalizeComSupportModule
#undef _idispatch
#undef _FatalAssertion
#undef _CurrentInterruptLevel
#undef _InterruptLevel
#undef _ExceptionLevel
#undef _AnotherExceptionFilter
#undef _SystemMutex
#undef _SystemMutexOwnerThreadID
#undef _SystemMutexLastOwnerThreadID
#undef _enter_system_critical_section
#undef _leave_system_critiacl_section
#undef _wait_for_thread_suspension_completion
#undef _isns_int
#undef _InterruptHandlerWrapper
#undef _def_int
#undef _ini_int
#undef _fin_int
#undef _ras_int
#undef _sns_int
#undef _sch_int
#undef _LockerThreadID
#undef _HALExceptionHandler
#undef _ini_exc
#undef _fin_exc
#undef _DebugOutDialogProc
#undef _InitializeDebugServices
#undef _FinalizeDebugServices
#undef _decode_waitstatus
#undef _decode_taskstatus
#undef _ProcessInstance
#undef _PrimaryThreadHandle
#undef _PrimaryDialogHandle
#undef _CurrentRunningThreadHandle
#undef _ShutdownPostponementRequest
#undef _WorkerThreadHandle
#undef _DestructionProcedureQueue
#undef _KernelStarter
#undef _TaskExceptionPerformer
#undef _ForceShutdownHandler
#undef _PrimaryDialogCommandHandler
#undef _HALMessageHandler
#undef _PrimaryDialogProc

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_UNRENAME_H_ */
