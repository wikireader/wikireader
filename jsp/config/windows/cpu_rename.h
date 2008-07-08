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
 *  @(#) $Id: cpu_rename.h,v 1.3 2003/07/01 13:49:51 takayuki Exp $
 */

/* This file is generated from cpu_rename.def by genrename. */

#ifndef _CPU_RENAME_H_
#define _CPU_RENAME_H_

#ifndef OMIT_RENAME

#define activate_r		_kernel_activate_r
#define ret_int			_kernel_ret_int
#define ret_exc			_kernel_ret_exc
#define NotifyDialogProc	_kernel_NotifyDialogProc
#define InitializeComSupportModule	_kernel_InitializeComSupportModule
#define FinalizeComSupportModule	_kernel_FinalizeComSupportModule
#define idispatch		_kernel_idispatch
#define FatalAssertion		_kernel_FatalAssertion
#define CurrentInterruptLevel	_kernel_CurrentInterruptLevel
#define InterruptLevel		_kernel_InterruptLevel
#define ExceptionLevel		_kernel_ExceptionLevel
#define AnotherExceptionFilter	_kernel_AnotherExceptionFilter
#define SystemMutex		_kernel_SystemMutex
#define SystemMutexOwnerThreadID	_kernel_SystemMutexOwnerThreadID
#define SystemMutexLastOwnerThreadID	_kernel_SystemMutexLastOwnerThreadID
#define enter_system_critical_section	_kernel_enter_system_critical_section
#define leave_system_critiacl_section	_kernel_leave_system_critiacl_section
#define wait_for_thread_suspension_completion	_kernel_wait_for_thread_suspension_completion
#define isns_int		_kernel_isns_int
#define InterruptHandlerWrapper	_kernel_InterruptHandlerWrapper
#define def_int			_kernel_def_int
#define ini_int			_kernel_ini_int
#define fin_int			_kernel_fin_int
#define ras_int			_kernel_ras_int
#define sns_int			_kernel_sns_int
#define sch_int			_kernel_sch_int
#define LockerThreadID		_kernel_LockerThreadID
#define HALExceptionHandler	_kernel_HALExceptionHandler
#define ini_exc			_kernel_ini_exc
#define fin_exc			_kernel_fin_exc
#define DebugOutDialogProc	_kernel_DebugOutDialogProc
#define InitializeDebugServices	_kernel_InitializeDebugServices
#define FinalizeDebugServices	_kernel_FinalizeDebugServices
#define decode_waitstatus	_kernel_decode_waitstatus
#define decode_taskstatus	_kernel_decode_taskstatus
#define ProcessInstance		_kernel_ProcessInstance
#define PrimaryThreadHandle	_kernel_PrimaryThreadHandle
#define PrimaryDialogHandle	_kernel_PrimaryDialogHandle
#define CurrentRunningThreadHandle	_kernel_CurrentRunningThreadHandle
#define ShutdownPostponementRequest	_kernel_ShutdownPostponementRequest
#define WorkerThreadHandle	_kernel_WorkerThreadHandle
#define DestructionProcedureQueue	_kernel_DestructionProcedureQueue
#define KernelStarter		_kernel_KernelStarter
#define TaskExceptionPerformer	_kernel_TaskExceptionPerformer
#define ForceShutdownHandler	_kernel_ForceShutdownHandler
#define PrimaryDialogCommandHandler	_kernel_PrimaryDialogCommandHandler
#define HALMessageHandler	_kernel_HALMessageHandler
#define PrimaryDialogProc	_kernel_PrimaryDialogProc

#ifdef LABEL_ASM

#define _activate_r		__kernel_activate_r
#define _ret_int		__kernel_ret_int
#define _ret_exc		__kernel_ret_exc
#define _NotifyDialogProc	__kernel_NotifyDialogProc
#define _InitializeComSupportModule	__kernel_InitializeComSupportModule
#define _FinalizeComSupportModule	__kernel_FinalizeComSupportModule
#define _idispatch		__kernel_idispatch
#define _FatalAssertion		__kernel_FatalAssertion
#define _CurrentInterruptLevel	__kernel_CurrentInterruptLevel
#define _InterruptLevel		__kernel_InterruptLevel
#define _ExceptionLevel		__kernel_ExceptionLevel
#define _AnotherExceptionFilter	__kernel_AnotherExceptionFilter
#define _SystemMutex		__kernel_SystemMutex
#define _SystemMutexOwnerThreadID	__kernel_SystemMutexOwnerThreadID
#define _SystemMutexLastOwnerThreadID	__kernel_SystemMutexLastOwnerThreadID
#define _enter_system_critical_section	__kernel_enter_system_critical_section
#define _leave_system_critiacl_section	__kernel_leave_system_critiacl_section
#define _wait_for_thread_suspension_completion	__kernel_wait_for_thread_suspension_completion
#define _isns_int		__kernel_isns_int
#define _InterruptHandlerWrapper	__kernel_InterruptHandlerWrapper
#define _def_int		__kernel_def_int
#define _ini_int		__kernel_ini_int
#define _fin_int		__kernel_fin_int
#define _ras_int		__kernel_ras_int
#define _sns_int		__kernel_sns_int
#define _sch_int		__kernel_sch_int
#define _LockerThreadID		__kernel_LockerThreadID
#define _HALExceptionHandler	__kernel_HALExceptionHandler
#define _ini_exc		__kernel_ini_exc
#define _fin_exc		__kernel_fin_exc
#define _DebugOutDialogProc	__kernel_DebugOutDialogProc
#define _InitializeDebugServices	__kernel_InitializeDebugServices
#define _FinalizeDebugServices	__kernel_FinalizeDebugServices
#define _decode_waitstatus	__kernel_decode_waitstatus
#define _decode_taskstatus	__kernel_decode_taskstatus
#define _ProcessInstance	__kernel_ProcessInstance
#define _PrimaryThreadHandle	__kernel_PrimaryThreadHandle
#define _PrimaryDialogHandle	__kernel_PrimaryDialogHandle
#define _CurrentRunningThreadHandle	__kernel_CurrentRunningThreadHandle
#define _ShutdownPostponementRequest	__kernel_ShutdownPostponementRequest
#define _WorkerThreadHandle	__kernel_WorkerThreadHandle
#define _DestructionProcedureQueue	__kernel_DestructionProcedureQueue
#define _KernelStarter		__kernel_KernelStarter
#define _TaskExceptionPerformer	__kernel_TaskExceptionPerformer
#define _ForceShutdownHandler	__kernel_ForceShutdownHandler
#define _PrimaryDialogCommandHandler	__kernel_PrimaryDialogCommandHandler
#define _HALMessageHandler	__kernel_HALMessageHandler
#define _PrimaryDialogProc	__kernel_PrimaryDialogProc

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _CPU_RENAME_H_ */
