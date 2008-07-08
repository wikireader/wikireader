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
 *  @(#) $Id: devicecontrol.cpp,v 1.4 2003/12/20 09:19:59 takayuki Exp $
 */

#include "stdafx.h"
#include "device.h"
#include "devicecontrol.h"

/////////////////////////////////////////////////////////////////////////////
// CDeviceControl

const CLSID CLSID_Device = {0xFEE2A68B,0x7F98,0x40E0,{0x98,0x53,0x18,0x3E,0xE6,0x8B,0xC7,0xF8}};

STDMETHODIMP CDeviceControl::Connect()
{
    if(!FAILED(manager.CoCreateInstance(CLSID_Device)) && !FAILED(manager.Advise((_IDeviceEvents *)this,__uuidof(_IDeviceEvents),&cookie))) {
        STARTUPINFO info;

        info.cb = sizeof(info);
        GetStartupInfo(&info);

        devname.Empty();
        devname.Append(info.lpTitle);
        manager->SetDeviceName(devname);
        
        return S_OK;
    }

    if(! !manager)
        manager.Release();

    return E_FAIL;
}

STDMETHODIMP CDeviceControl::Close()
{
    if(!manager)
        return E_FAIL;

    AtlUnadvise(manager,__uuidof(_IDeviceEvents),cookie);
    manager.Release();
    cookie = 0;
    return S_OK;
}

STDMETHODIMP CDeviceControl::get_Valid(BOOL *pVal)
{
    *pVal = !manager ? FALSE : ~0;
    return S_OK;
}

STDMETHODIMP CDeviceControl::RaiseInterrupt(long inhno)
{
    if(!manager)
        return E_FAIL;
    
    return manager->RaiseInterrupt(inhno);
}

STDMETHODIMP CDeviceControl::Map(long address, long size)
{
    if(!manager)
        return E_FAIL;

    return manager->Map(address, size);
}

STDMETHODIMP CDeviceControl::Unmap(long address)
{
    if(!manager)
        return E_FAIL;

    return manager->Unmap(address);
}


STDMETHODIMP CDeviceControl::get_IsKernelStarted(BOOL *pVal)
{
    *pVal = KernelStarted ? ~0 : 0;
    return S_OK;
}

STDMETHODIMP CDeviceControl::get_Offset(long *pVal)
{
    *pVal = Offset;
    return S_OK;
}

STDMETHODIMP CDeviceControl::put_Offset(long newVal)
{
    if(newVal >= Limitation)
        return E_FAIL;

    Offset = newVal;

    return S_OK;
}

STDMETHODIMP CDeviceControl::get_AccessSize(short *pVal)
{
    *pVal = AccessSize;
    return S_OK;
}

STDMETHODIMP CDeviceControl::put_AccessSize(short newVal)
{
    switch(newVal)
    {
    case 1: case 2: case 4:
        break;
    default:
        return E_FAIL;
    }

    AccessSize = newVal;
    return S_OK;
}

STDMETHODIMP CDeviceControl::get_Value(long *pVal)
{
    if(DataPointer == 0)
        return E_FAIL;

    *pVal = 0;
    CopyMemory(pVal, DataPointer + Offset, AccessSize);

    Offset += AccessSize;
    if(Offset >= Limitation)
        Offset = Limitation -1;
    
    return S_OK;
}

STDMETHODIMP CDeviceControl::put_Value(long newVal)
{
    if(DataPointer == 0)
        return E_FAIL;

    CopyMemory(DataPointer + Offset, &newVal, AccessSize);

    Offset += AccessSize;
    if(Offset >= Limitation)
        Offset = Limitation -1;

    return S_OK;
}

STDMETHODIMP CDeviceControl::OnRead(long address,long sz,byte __RPC_FAR data[])
{
    if(!KernelStarted)
        this->OnKernelStart();

    DataPointer = (unsigned char *)data;
    Limitation = sz;
    Offset = 0;

    Fire_OnRead(address, sz);
    
    DataPointer = 0;
    return S_OK;
}

STDMETHODIMP CDeviceControl::OnWrite(long address,long sz,byte __RPC_FAR data[])
{
    if(!KernelStarted)
        this->OnKernelStart();

    DataPointer = (unsigned char *)data;
    Limitation = sz;
    Offset = 0;
    Fire_OnWrite(address,sz);
    DataPointer = 0;
    return S_OK;
}

STDMETHODIMP CDeviceControl::OnKernelStart()
{
    KernelStarted = true;
    Fire_OnKernelStart();
    return S_OK;
}

STDMETHODIMP CDeviceControl::OnKernelExit()
{
    KernelStarted = false;
    Fire_OnKernelExit();
    return S_OK;
}

STDMETHODIMP CDeviceControl::get_DeviceName(BSTR* pVal)
{
    if(!manager)
        return E_FAIL;

    devname.CopyTo(pVal);
    return S_OK;
}

STDMETHODIMP CDeviceControl::put_DeviceName(BSTR newVal)
{
    if(!manager)
        return E_FAIL;

    devname = newVal;
    manager->SetDeviceName(devname);
    return S_OK;
}
