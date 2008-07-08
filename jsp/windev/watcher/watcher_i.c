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
 *  @(#) $Id: watcher_i.c,v 1.3 2003/06/30 15:58:39 takayuki Exp $
 */

/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Nov 01 23:33:21 2002
 */
/* Compiler settings for C:\Documents and Settings\Takayuki\My Documents\Visual Studio Projects\JSP Windows Logging Mechanism\watcher\watcher.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IKernelLogControl = {0x1FFB399D,0x1058,0x40B4,{0xAF,0x62,0xEA,0x43,0xCD,0xDD,0x79,0xB0}};


const IID LIBID_WATCHERLib = {0xA242E427,0x5E2F,0x4447,{0xAC,0x87,0x0C,0x7C,0xC3,0x82,0xCB,0xFE}};


const IID DIID__IKernelLogControlEvents = {0x99CCF4E3,0x5D17,0x477E,{0xBA,0x7B,0xF5,0xDE,0x79,0x6C,0x8E,0x13}};


const IID IID_IDirectoryMap = {0x80AA7D6B,0xF115,0x4ACE,{0x97,0xF6,0x55,0x39,0x1B,0xA2,0x79,0x97}};


const CLSID CLSID_KernelLogControl = {0x3474A699,0x17D5,0x4E00,{0x80,0x68,0x6D,0x66,0x08,0xF3,0xDF,0x8A}};


const CLSID CLSID_DirectoryMap = {0xBA0030F5,0x042A,0x43DC,{0xBF,0x89,0x1B,0x34,0x3A,0x38,0xE3,0x20}};


#ifdef __cplusplus
}
#endif

