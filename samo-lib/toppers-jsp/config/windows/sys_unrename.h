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
 *  @(#) $Id: sys_unrename.h,v 1.4 2005/07/06 03:10:29 honda Exp $
 */

/* This file is generated from sys_rename.def by genrename. */

#ifdef _SYS_UNRENAME_H_
#undef _SYS_UNRENAME_H_

#ifndef OMIT_RENAME

#undef siopcb_table
#undef KeyEventTrapper
#undef SelectConsoleFont
#undef ConsoleCommandHandler
#undef ConsoleProc
#undef SerialConsole_FinalRelease
#undef CreateSerialConsole
#undef SerialConsole_PutChar
#undef SerialConsole_PushChar
#undef WinConsole_ReceiverThread
#undef WinConsole_CreatePort
#undef WinConsole_PutChar
#undef WinConsole_ClosePort
#undef ScreenBuffer_ReceiverThread
#undef ScreenBuffer_CreatePort
#undef ScreenBuffer_PutChar
#undef ScreenBuffer_ClosePort
#undef SerialRawPutc
#undef MultiMediaTimerCallbackFunction

#ifdef LABEL_ASM

#undef _siopcb_table
#undef _KeyEventTrapper
#undef _SelectConsoleFont
#undef _ConsoleCommandHandler
#undef _ConsoleProc
#undef _SerialConsole_FinalRelease
#undef _CreateSerialConsole
#undef _SerialConsole_PutChar
#undef _SerialConsole_PushChar
#undef _WinConsole_ReceiverThread
#undef _WinConsole_CreatePort
#undef _WinConsole_PutChar
#undef _WinConsole_ClosePort
#undef _ScreenBuffer_ReceiverThread
#undef _ScreenBuffer_CreatePort
#undef _ScreenBuffer_PutChar
#undef _ScreenBuffer_ClosePort
#undef _SerialRawPutc
#undef _MultiMediaTimerCallbackFunction

#endif /* LABEL_ASM */
#endif /* OMIT_RENAME */
#endif /* _SYS_UNRENAME_H_ */
