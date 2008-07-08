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
 *  @(#) $Id: sys_rename.h,v 1.3 2003/07/01 13:49:51 takayuki Exp $
 */

/* This file is generated from sys_rename.def by genrename. */

#ifndef _SYS_RENAME_H_
#define _SYS_RENAME_H_

#ifndef OMIT_RENAME

#define siopcb_table		_kernel_siopcb_table
#define KeyEventTrapper		_kernel_KeyEventTrapper
#define SelectConsoleFont	_kernel_SelectConsoleFont
#define ConsoleCommandHandler	_kernel_ConsoleCommandHandler
#define ConsoleProc		_kernel_ConsoleProc
#define SerialConsole_FinalRelease	_kernel_SerialConsole_FinalRelease
#define CreateSerialConsole	_kernel_CreateSerialConsole
#define SerialConsole_PutChar	_kernel_SerialConsole_PutChar
#define SerialConsole_PushChar	_kernel_SerialConsole_PushChar
#define WinConsole_ReceiverThread	_kernel_WinConsole_ReceiverThread
#define WinConsole_CreatePort	_kernel_WinConsole_CreatePort
#define WinConsole_PutChar	_kernel_WinConsole_PutChar
#define WinConsole_ClosePort	_kernel_WinConsole_ClosePort
#define ScreenBuffer_ReceiverThread	_kernel_ScreenBuffer_ReceiverThread
#define ScreenBuffer_CreatePort	_kernel_ScreenBuffer_CreatePort
#define ScreenBuffer_PutChar	_kernel_ScreenBuffer_PutChar
#define ScreenBuffer_ClosePort	_kernel_ScreenBuffer_ClosePort
#define SerialRawPutc		_kernel_SerialRawPutc
#define MultiMediaTimerCallbackFunction	_kernel_MultiMediaTimerCallbackFunction

#ifdef LABEL_ASM

#define _siopcb_table		__kernel_siopcb_table
#define _KeyEventTrapper	__kernel_KeyEventTrapper
#define _SelectConsoleFont	__kernel_SelectConsoleFont
#define _ConsoleCommandHandler	__kernel_ConsoleCommandHandler
#define _ConsoleProc		__kernel_ConsoleProc
#define _SerialConsole_FinalRelease	__kernel_SerialConsole_FinalRelease
#define _CreateSerialConsole	__kernel_CreateSerialConsole
#define _SerialConsole_PutChar	__kernel_SerialConsole_PutChar
#define _SerialConsole_PushChar	__kernel_SerialConsole_PushChar
#define _WinConsole_ReceiverThread	__kernel_WinConsole_ReceiverThread
#define _WinConsole_CreatePort	__kernel_WinConsole_CreatePort
#define _WinConsole_PutChar	__kernel_WinConsole_PutChar
#define _WinConsole_ClosePort	__kernel_WinConsole_ClosePort
#define _ScreenBuffer_ReceiverThread	__kernel_ScreenBuffer_ReceiverThread
#define _ScreenBuffer_CreatePort	__kernel_ScreenBuffer_CreatePort
#define _ScreenBuffer_PutChar	__kernel_ScreenBuffer_PutChar
#define _ScreenBuffer_ClosePort	__kernel_ScreenBuffer_ClosePort
#define _SerialRawPutc		__kernel_SerialRawPutc
#define _MultiMediaTimerCallbackFunction	__kernel_MultiMediaTimerCallbackFunction

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _SYS_RENAME_H_ */
