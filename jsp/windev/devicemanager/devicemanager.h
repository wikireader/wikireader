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
 *  @(#) $Id: devicemanager.h,v 1.6 2003/06/30 15:58:39 takayuki Exp $
 */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Aug 20 16:26:30 2002
 */
/* Compiler settings for C:\Documents and Settings\Takayuki\My Documents\Visual Studio Projects\JSP Windows Logging Mechanism\windev\devicemanager\devicemanager.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __devicemanager_h__
#define __devicemanager_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IInformation_FWD_DEFINED__
#define __IInformation_FWD_DEFINED__
typedef interface IInformation IInformation;
#endif 	/* __IInformation_FWD_DEFINED__ */


#ifndef __IDevice_FWD_DEFINED__
#define __IDevice_FWD_DEFINED__
typedef interface IDevice IDevice;
#endif 	/* __IDevice_FWD_DEFINED__ */


#ifndef ___IDeviceEvents_FWD_DEFINED__
#define ___IDeviceEvents_FWD_DEFINED__
typedef interface _IDeviceEvents _IDeviceEvents;
#endif 	/* ___IDeviceEvents_FWD_DEFINED__ */


#ifndef __IKernel_FWD_DEFINED__
#define __IKernel_FWD_DEFINED__
typedef interface IKernel IKernel;
#endif 	/* __IKernel_FWD_DEFINED__ */


#ifndef ___IKernelEvents_FWD_DEFINED__
#define ___IKernelEvents_FWD_DEFINED__
typedef interface _IKernelEvents _IKernelEvents;
#endif 	/* ___IKernelEvents_FWD_DEFINED__ */


#ifndef ___IInformationEvents_FWD_DEFINED__
#define ___IInformationEvents_FWD_DEFINED__
typedef interface _IInformationEvents _IInformationEvents;
#endif 	/* ___IInformationEvents_FWD_DEFINED__ */


#ifndef __IKernelLog_FWD_DEFINED__
#define __IKernelLog_FWD_DEFINED__
typedef interface IKernelLog IKernelLog;
#endif 	/* __IKernelLog_FWD_DEFINED__ */


#ifndef ___IKernelLogEvents_FWD_DEFINED__
#define ___IKernelLogEvents_FWD_DEFINED__
typedef interface _IKernelLogEvents _IKernelLogEvents;
#endif 	/* ___IKernelLogEvents_FWD_DEFINED__ */


#ifndef __Device_FWD_DEFINED__
#define __Device_FWD_DEFINED__

#ifdef __cplusplus
typedef class Device Device;
#else
typedef struct Device Device;
#endif /* __cplusplus */

#endif 	/* __Device_FWD_DEFINED__ */


#ifndef __Information_FWD_DEFINED__
#define __Information_FWD_DEFINED__

#ifdef __cplusplus
typedef class Information Information;
#else
typedef struct Information Information;
#endif /* __cplusplus */

#endif 	/* __Information_FWD_DEFINED__ */


#ifndef __Kernel_FWD_DEFINED__
#define __Kernel_FWD_DEFINED__

#ifdef __cplusplus
typedef class Kernel Kernel;
#else
typedef struct Kernel Kernel;
#endif /* __cplusplus */

#endif 	/* __Kernel_FWD_DEFINED__ */


#ifndef __KernelLog_FWD_DEFINED__
#define __KernelLog_FWD_DEFINED__

#ifdef __cplusplus
typedef class KernelLog KernelLog;
#else
typedef struct KernelLog KernelLog;
#endif /* __cplusplus */

#endif 	/* __KernelLog_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IInformation_INTERFACE_DEFINED__
#define __IInformation_INTERFACE_DEFINED__

/* interface IInformation */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0FDFFA99-68D5-4BE0-8984-FB6A3A2C0AAC")
    IInformation : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IInformationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IInformation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IInformation __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IInformation __RPC_FAR * This);
        
        END_INTERFACE
    } IInformationVtbl;

    interface IInformation
    {
        CONST_VTBL struct IInformationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInformation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInformation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInformation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInformation_INTERFACE_DEFINED__ */


#ifndef __IDevice_INTERFACE_DEFINED__
#define __IDevice_INTERFACE_DEFINED__

/* interface IDevice */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6575FAAB-8750-42D2-ADDD-4BE764AA2767")
    IDevice : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsValid( 
            /* [out] */ BOOL __RPC_FAR *valid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDeviceName( 
            /* [in] */ BSTR devname) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Map( 
            /* [in] */ unsigned long address,
            /* [in] */ unsigned long size) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Unmap( 
            /* [in] */ unsigned long address) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RaiseInterrupt( 
            /* [in] */ unsigned long inhno) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDevice __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDevice __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDevice __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsValid )( 
            IDevice __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *valid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDeviceName )( 
            IDevice __RPC_FAR * This,
            /* [in] */ BSTR devname);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Map )( 
            IDevice __RPC_FAR * This,
            /* [in] */ unsigned long address,
            /* [in] */ unsigned long size);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Unmap )( 
            IDevice __RPC_FAR * This,
            /* [in] */ unsigned long address);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RaiseInterrupt )( 
            IDevice __RPC_FAR * This,
            /* [in] */ unsigned long inhno);
        
        END_INTERFACE
    } IDeviceVtbl;

    interface IDevice
    {
        CONST_VTBL struct IDeviceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDevice_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDevice_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDevice_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDevice_IsValid(This,valid)	\
    (This)->lpVtbl -> IsValid(This,valid)

#define IDevice_SetDeviceName(This,devname)	\
    (This)->lpVtbl -> SetDeviceName(This,devname)

#define IDevice_Map(This,address,size)	\
    (This)->lpVtbl -> Map(This,address,size)

#define IDevice_Unmap(This,address)	\
    (This)->lpVtbl -> Unmap(This,address)

#define IDevice_RaiseInterrupt(This,inhno)	\
    (This)->lpVtbl -> RaiseInterrupt(This,inhno)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDevice_IsValid_Proxy( 
    IDevice __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *valid);


void __RPC_STUB IDevice_IsValid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDevice_SetDeviceName_Proxy( 
    IDevice __RPC_FAR * This,
    /* [in] */ BSTR devname);


void __RPC_STUB IDevice_SetDeviceName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDevice_Map_Proxy( 
    IDevice __RPC_FAR * This,
    /* [in] */ unsigned long address,
    /* [in] */ unsigned long size);


void __RPC_STUB IDevice_Map_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDevice_Unmap_Proxy( 
    IDevice __RPC_FAR * This,
    /* [in] */ unsigned long address);


void __RPC_STUB IDevice_Unmap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IDevice_RaiseInterrupt_Proxy( 
    IDevice __RPC_FAR * This,
    /* [in] */ unsigned long inhno);


void __RPC_STUB IDevice_RaiseInterrupt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDevice_INTERFACE_DEFINED__ */


#ifndef ___IDeviceEvents_INTERFACE_DEFINED__
#define ___IDeviceEvents_INTERFACE_DEFINED__

/* interface _IDeviceEvents */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID__IDeviceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DA93137D-FB52-4421-B95D-9077340AD03B")
    _IDeviceEvents : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnRead( 
            /* [in] */ long address,
            /* [in] */ long sz,
            /* [length_is][size_is][out] */ byte __RPC_FAR data[  ]) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnWrite( 
            /* [in] */ long address,
            /* [in] */ long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnKernelStart( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnKernelExit( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct _IDeviceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IDeviceEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IDeviceEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IDeviceEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnRead )( 
            _IDeviceEvents __RPC_FAR * This,
            /* [in] */ long address,
            /* [in] */ long sz,
            /* [length_is][size_is][out] */ byte __RPC_FAR data[  ]);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnWrite )( 
            _IDeviceEvents __RPC_FAR * This,
            /* [in] */ long address,
            /* [in] */ long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnKernelStart )( 
            _IDeviceEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnKernelExit )( 
            _IDeviceEvents __RPC_FAR * This);
        
        END_INTERFACE
    } _IDeviceEventsVtbl;

    interface _IDeviceEvents
    {
        CONST_VTBL struct _IDeviceEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IDeviceEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IDeviceEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IDeviceEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IDeviceEvents_OnRead(This,address,sz,data)	\
    (This)->lpVtbl -> OnRead(This,address,sz,data)

#define _IDeviceEvents_OnWrite(This,address,sz,data)	\
    (This)->lpVtbl -> OnWrite(This,address,sz,data)

#define _IDeviceEvents_OnKernelStart(This)	\
    (This)->lpVtbl -> OnKernelStart(This)

#define _IDeviceEvents_OnKernelExit(This)	\
    (This)->lpVtbl -> OnKernelExit(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IDeviceEvents_OnRead_Proxy( 
    _IDeviceEvents __RPC_FAR * This,
    /* [in] */ long address,
    /* [in] */ long sz,
    /* [length_is][size_is][out] */ byte __RPC_FAR data[  ]);


void __RPC_STUB _IDeviceEvents_OnRead_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IDeviceEvents_OnWrite_Proxy( 
    _IDeviceEvents __RPC_FAR * This,
    /* [in] */ long address,
    /* [in] */ long sz,
    /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);


void __RPC_STUB _IDeviceEvents_OnWrite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IDeviceEvents_OnKernelStart_Proxy( 
    _IDeviceEvents __RPC_FAR * This);


void __RPC_STUB _IDeviceEvents_OnKernelStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IDeviceEvents_OnKernelExit_Proxy( 
    _IDeviceEvents __RPC_FAR * This);


void __RPC_STUB _IDeviceEvents_OnKernelExit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* ___IDeviceEvents_INTERFACE_DEFINED__ */


#ifndef __IKernel_INTERFACE_DEFINED__
#define __IKernel_INTERFACE_DEFINED__

/* interface IKernel */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IKernel;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D3E42099-3FDD-4A78-BDBD-4E57D362F5ED")
    IKernel : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ unsigned long address,
            /* [in] */ unsigned long sz,
            /* [length_is][size_is][out] */ byte __RPC_FAR data[  ]) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsValid( 
            /* [out] */ BOOL __RPC_FAR *valid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [in] */ unsigned long address,
            /* [in] */ unsigned long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnLogEvent( 
            /* [in] */ long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IKernelVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IKernel __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IKernel __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IKernel __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read )( 
            IKernel __RPC_FAR * This,
            /* [in] */ unsigned long address,
            /* [in] */ unsigned long sz,
            /* [length_is][size_is][out] */ byte __RPC_FAR data[  ]);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsValid )( 
            IKernel __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *valid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Write )( 
            IKernel __RPC_FAR * This,
            /* [in] */ unsigned long address,
            /* [in] */ unsigned long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnLogEvent )( 
            IKernel __RPC_FAR * This,
            /* [in] */ long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);
        
        END_INTERFACE
    } IKernelVtbl;

    interface IKernel
    {
        CONST_VTBL struct IKernelVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKernel_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IKernel_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IKernel_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IKernel_Read(This,address,sz,data)	\
    (This)->lpVtbl -> Read(This,address,sz,data)

#define IKernel_IsValid(This,valid)	\
    (This)->lpVtbl -> IsValid(This,valid)

#define IKernel_Write(This,address,sz,data)	\
    (This)->lpVtbl -> Write(This,address,sz,data)

#define IKernel_OnLogEvent(This,sz,data)	\
    (This)->lpVtbl -> OnLogEvent(This,sz,data)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IKernel_Read_Proxy( 
    IKernel __RPC_FAR * This,
    /* [in] */ unsigned long address,
    /* [in] */ unsigned long sz,
    /* [length_is][size_is][out] */ byte __RPC_FAR data[  ]);


void __RPC_STUB IKernel_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IKernel_IsValid_Proxy( 
    IKernel __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *valid);


void __RPC_STUB IKernel_IsValid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IKernel_Write_Proxy( 
    IKernel __RPC_FAR * This,
    /* [in] */ unsigned long address,
    /* [in] */ unsigned long sz,
    /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);


void __RPC_STUB IKernel_Write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IKernel_OnLogEvent_Proxy( 
    IKernel __RPC_FAR * This,
    /* [in] */ long sz,
    /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);


void __RPC_STUB IKernel_OnLogEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IKernel_INTERFACE_DEFINED__ */


#ifndef ___IKernelEvents_INTERFACE_DEFINED__
#define ___IKernelEvents_INTERFACE_DEFINED__

/* interface _IKernelEvents */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID__IKernelEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1353969D-E84F-463F-B211-337E9BCFB99E")
    _IKernelEvents : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnInterruptRequest( 
            /* [in] */ unsigned long inhno) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct _IKernelEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IKernelEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IKernelEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IKernelEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnInterruptRequest )( 
            _IKernelEvents __RPC_FAR * This,
            /* [in] */ unsigned long inhno);
        
        END_INTERFACE
    } _IKernelEventsVtbl;

    interface _IKernelEvents
    {
        CONST_VTBL struct _IKernelEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IKernelEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IKernelEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IKernelEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IKernelEvents_OnInterruptRequest(This,inhno)	\
    (This)->lpVtbl -> OnInterruptRequest(This,inhno)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IKernelEvents_OnInterruptRequest_Proxy( 
    _IKernelEvents __RPC_FAR * This,
    /* [in] */ unsigned long inhno);


void __RPC_STUB _IKernelEvents_OnInterruptRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* ___IKernelEvents_INTERFACE_DEFINED__ */


#ifndef ___IInformationEvents_INTERFACE_DEFINED__
#define ___IInformationEvents_INTERFACE_DEFINED__

/* interface _IInformationEvents */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID__IInformationEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("11E125BE-FC60-4DC9-8393-DC393B556D06")
    _IInformationEvents : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnDeviceChanged( 
            short reason,
            long devid,
            long extra) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnKernelChanged( 
            short reason) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct _IInformationEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IInformationEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IInformationEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IInformationEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnDeviceChanged )( 
            _IInformationEvents __RPC_FAR * This,
            short reason,
            long devid,
            long extra);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnKernelChanged )( 
            _IInformationEvents __RPC_FAR * This,
            short reason);
        
        END_INTERFACE
    } _IInformationEventsVtbl;

    interface _IInformationEvents
    {
        CONST_VTBL struct _IInformationEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IInformationEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IInformationEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IInformationEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IInformationEvents_OnDeviceChanged(This,reason,devid,extra)	\
    (This)->lpVtbl -> OnDeviceChanged(This,reason,devid,extra)

#define _IInformationEvents_OnKernelChanged(This,reason)	\
    (This)->lpVtbl -> OnKernelChanged(This,reason)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IInformationEvents_OnDeviceChanged_Proxy( 
    _IInformationEvents __RPC_FAR * This,
    short reason,
    long devid,
    long extra);


void __RPC_STUB _IInformationEvents_OnDeviceChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IInformationEvents_OnKernelChanged_Proxy( 
    _IInformationEvents __RPC_FAR * This,
    short reason);


void __RPC_STUB _IInformationEvents_OnKernelChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* ___IInformationEvents_INTERFACE_DEFINED__ */


#ifndef __IKernelLog_INTERFACE_DEFINED__
#define __IKernelLog_INTERFACE_DEFINED__

/* interface IKernelLog */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IKernelLog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("395F900A-AC7E-4A78-9BC1-EE5EF76254FF")
    IKernelLog : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IKernelLogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IKernelLog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IKernelLog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IKernelLog __RPC_FAR * This);
        
        END_INTERFACE
    } IKernelLogVtbl;

    interface IKernelLog
    {
        CONST_VTBL struct IKernelLogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IKernelLog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IKernelLog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IKernelLog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IKernelLog_INTERFACE_DEFINED__ */


#ifndef ___IKernelLogEvents_INTERFACE_DEFINED__
#define ___IKernelLogEvents_INTERFACE_DEFINED__

/* interface _IKernelLogEvents */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID__IKernelLogEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("35E35399-55ED-45FC-8F0B-4A1BC6CEA3F0")
    _IKernelLogEvents : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnKernelStart( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnKernelExit( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnLogEvent( 
            /* [in] */ long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct _IKernelLogEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IKernelLogEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IKernelLogEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IKernelLogEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnKernelStart )( 
            _IKernelLogEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnKernelExit )( 
            _IKernelLogEvents __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnLogEvent )( 
            _IKernelLogEvents __RPC_FAR * This,
            /* [in] */ long sz,
            /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);
        
        END_INTERFACE
    } _IKernelLogEventsVtbl;

    interface _IKernelLogEvents
    {
        CONST_VTBL struct _IKernelLogEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IKernelLogEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IKernelLogEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IKernelLogEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IKernelLogEvents_OnKernelStart(This)	\
    (This)->lpVtbl -> OnKernelStart(This)

#define _IKernelLogEvents_OnKernelExit(This)	\
    (This)->lpVtbl -> OnKernelExit(This)

#define _IKernelLogEvents_OnLogEvent(This,sz,data)	\
    (This)->lpVtbl -> OnLogEvent(This,sz,data)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IKernelLogEvents_OnKernelStart_Proxy( 
    _IKernelLogEvents __RPC_FAR * This);


void __RPC_STUB _IKernelLogEvents_OnKernelStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IKernelLogEvents_OnKernelExit_Proxy( 
    _IKernelLogEvents __RPC_FAR * This);


void __RPC_STUB _IKernelLogEvents_OnKernelExit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE _IKernelLogEvents_OnLogEvent_Proxy( 
    _IKernelLogEvents __RPC_FAR * This,
    /* [in] */ long sz,
    /* [length_is][size_is][in] */ byte __RPC_FAR data[  ]);


void __RPC_STUB _IKernelLogEvents_OnLogEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* ___IKernelLogEvents_INTERFACE_DEFINED__ */



#ifndef __ATLDEVICEMANAGERLib_LIBRARY_DEFINED__
#define __ATLDEVICEMANAGERLib_LIBRARY_DEFINED__

/* library ATLDEVICEMANAGERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ATLDEVICEMANAGERLib;

EXTERN_C const CLSID CLSID_Device;

#ifdef __cplusplus

class DECLSPEC_UUID("FEE2A68B-7F98-40E0-9853-183EE68BC7F8")
Device;
#endif

EXTERN_C const CLSID CLSID_Information;

#ifdef __cplusplus

class DECLSPEC_UUID("D7AAF617-008A-4961-BB51-041CFD490ED3")
Information;
#endif

EXTERN_C const CLSID CLSID_Kernel;

#ifdef __cplusplus

class DECLSPEC_UUID("51789667-9F20-40AF-AF7F-9856325DFB0B")
Kernel;
#endif

EXTERN_C const CLSID CLSID_KernelLog;

#ifdef __cplusplus

class DECLSPEC_UUID("4BD327FC-9E4A-4A5D-9503-27C979A8E802")
KernelLog;
#endif
#endif /* __ATLDEVICEMANAGERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
