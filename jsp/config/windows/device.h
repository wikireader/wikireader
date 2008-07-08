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
 *  @(#) $Id: device.h,v 1.9 2003/12/15 07:19:22 takayuki Exp $
 */

#ifndef __DEVICE_H
#define __DEVICE_H

#include <cpu_rename.h>
#include <s_services.h>

#ifdef DEVICE_ENABLE

#define MAX_MSGSIZE 256

#define DEVICE_TIMEOUT INFINITE

#define OnDevice

extern struct tagDeviceGlobalVariables
{
    CRITICAL_SECTION cs;
    HANDLE ManagerSlot;
    HANDLE ListenerThread;
    HANDLE Blocker;
    HANDLE ThreadBlocker;
    HANDLE ReplySlot;
    DWORD ListenerThreadID;
} DeviceVars;

void InitializeDeviceSupportModule(void);
void FinalizeDeviceSupportModule(void);

int DeviceRead(unsigned long address, unsigned long size, void * storage);
int DeviceWrite(unsigned long address, unsigned long size, void * storage);


#define SIL_ENDIAN SIL_ENDIAN_LITTLE

/*
 *  エンディアンの反転
 */
#ifndef SIL_REV_ENDIAN_H
#define	SIL_REV_ENDIAN_H(data) \
    ((VH)((((UH)(data) & 0xff) << 8) | (((UH)(data) >> 8) & 0xff)))
#endif /* SIL_REV_ENDIAN_H */

#ifndef SIL_REV_ENDIAN_W
#define	SIL_REV_ENDIAN_W(data) \
    ((VW)((((UW)(data) & 0xff) << 24) | (((UW)(data) & 0xff00) << 8) \
    | (((UW)(data)>> 8) & 0xff00) | (((UW)(data) >> 24) & 0xff)))
#endif /* SIL_REV_ENDIAN_H */

/*
 *  自然なエンディアンでの読出し
 */
Inline VB
sil_reb_mem(VP mem)
{
    VB data;
    DeviceRead((unsigned long)mem, sizeof(data), &data);
    return data;
}

Inline VH
sil_reh_mem(VP mem)
{
    VH data;
    DeviceRead((unsigned long)mem, sizeof(data), &data);
    return data;
}

Inline VW
sil_rew_mem(VP mem)
{
    VW data;
    DeviceRead((unsigned long)mem, sizeof(data), &data);
    return data;
}

Inline void
sil_rek_mem(VP mem, VP data, UINT len)
{	DeviceRead((unsigned long)mem, len, data);   }

/*
 *  リトルエンディアンでの読出し（リトルエンディアンプロセッサ）
 */
#define	sil_reh_lem(mem)	sil_reh_mem(mem)
#define	sil_rew_lem(mem)	sil_rew_mem(mem)

/*
 *  ビッグエンディアンでの読出し（リトルエンディアンプロセッサ）
 */
Inline VH
sil_reh_bem(VP mem)
{   return SIL_REV_ENDIAN_H(sil_reh_mem(mem));   }

Inline VW
sil_rew_bem(VP mem)
{   return SIL_REV_ENDIAN_W(sil_rew_mem(mem));   }

/*
 *  自然なエンディアンでの書込み
 */
Inline void
sil_wrb_mem(VP mem, VB data)
{   DeviceWrite((unsigned long)mem, sizeof(data), &data);   }

Inline void
sil_wrh_mem(VP mem, VH data)
{   DeviceWrite((unsigned long)mem, sizeof(data), &data);   }

Inline void
sil_wrw_mem(VP mem, VW data)
{   DeviceWrite((unsigned long)mem, sizeof(data), &data);   }

Inline void
sil_wrk_mem(VP mem, VP data, UINT len)
{	DeviceWrite((unsigned long)mem, len, data);   }

#define	sil_wrh_lem(mem, data)	sil_wrh_mem(mem, data)
#define	sil_wrw_lem(mem, data)	sil_wrw_mem(mem, data)

/*
 *  ビッグエンディアンでの書込み（リトルエンディアンプロセッサ）
 */
Inline void
sil_wrh_bem(VP mem, VH data)
{
    data = SIL_REV_ENDIAN_H(data);
    DeviceWrite((unsigned long)mem, sizeof(data), &data);   		
}

Inline void
sil_wrw_bem(VP mem, VW data)
{
    data = SIL_REV_ENDIAN_W(data);
    DeviceWrite((unsigned long)mem, sizeof(data), &data);   		
}

#else // DEVICE_ENABLE

//デバイスは利用しない

#define OnDevice /##/
#define InitializeDeviceSupportModule /##/
#define FinalizeDeviceSupportModule /##/

/*
#define DeviceRead(address, size, storage) \
    memcpy(storage, (void *)address, size)

#define DeviceWrite(address, size, storage) \
    memcpy((void *)address, storage, size)
*/

#define DeviceRead /##/
#define DeviceWrite /##/

#endif

#endif

