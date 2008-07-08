

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Oct 21 19:55:03 2003
 */
/* Compiler settings for .\device.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __device_h__
#define __device_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDeviceControl_FWD_DEFINED__
#define __IDeviceControl_FWD_DEFINED__
typedef interface IDeviceControl IDeviceControl;
#endif 	/* __IDeviceControl_FWD_DEFINED__ */


#ifndef ___IDeviceControlEvents_FWD_DEFINED__
#define ___IDeviceControlEvents_FWD_DEFINED__
typedef interface _IDeviceControlEvents _IDeviceControlEvents;
#endif 	/* ___IDeviceControlEvents_FWD_DEFINED__ */


#ifndef __DeviceControl_FWD_DEFINED__
#define __DeviceControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class DeviceControl DeviceControl;
#else
typedef struct DeviceControl DeviceControl;
#endif /* __cplusplus */

#endif 	/* __DeviceControl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IDeviceControl_INTERFACE_DEFINED__
#define __IDeviceControl_INTERFACE_DEFINED__

/* interface IDeviceControl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IDeviceControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6998EBB2-678E-4891-8BD0-C34F165488AC")
    IDeviceControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Valid( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RaiseInterrupt( 
            /* [in] */ long inhno) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Map( 
            /* [in] */ long address,
            /* [in] */ long size) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Unmap( 
            /* [in] */ long address) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsKernelStarted( 
            /* [retval][out] */ BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Offset( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Offset( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AccessSize( 
            /* [retval][out] */ short *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AccessSize( 
            /* [in] */ short newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeviceName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DeviceName( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeviceControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeviceControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeviceControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeviceControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDeviceControl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDeviceControl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDeviceControl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDeviceControl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IDeviceControl * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IDeviceControl * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Valid )( 
            IDeviceControl * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RaiseInterrupt )( 
            IDeviceControl * This,
            /* [in] */ long inhno);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Map )( 
            IDeviceControl * This,
            /* [in] */ long address,
            /* [in] */ long size);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Unmap )( 
            IDeviceControl * This,
            /* [in] */ long address);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsKernelStarted )( 
            IDeviceControl * This,
            /* [retval][out] */ BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Offset )( 
            IDeviceControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Offset )( 
            IDeviceControl * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_AccessSize )( 
            IDeviceControl * This,
            /* [retval][out] */ short *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_AccessSize )( 
            IDeviceControl * This,
            /* [in] */ short newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            IDeviceControl * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Value )( 
            IDeviceControl * This,
            /* [in] */ long newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeviceName )( 
            IDeviceControl * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DeviceName )( 
            IDeviceControl * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IDeviceControlVtbl;

    interface IDeviceControl
    {
        CONST_VTBL struct IDeviceControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeviceControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeviceControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeviceControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeviceControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDeviceControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDeviceControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDeviceControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDeviceControl_Connect(This)	\
    (This)->lpVtbl -> Connect(This)

#define IDeviceControl_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IDeviceControl_get_Valid(This,pVal)	\
    (This)->lpVtbl -> get_Valid(This,pVal)

#define IDeviceControl_RaiseInterrupt(This,inhno)	\
    (This)->lpVtbl -> RaiseInterrupt(This,inhno)

#define IDeviceControl_Map(This,address,size)	\
    (This)->lpVtbl -> Map(This,address,size)

#define IDeviceControl_Unmap(This,address)	\
    (This)->lpVtbl -> Unmap(This,address)

#define IDeviceControl_get_IsKernelStarted(This,pVal)	\
    (This)->lpVtbl -> get_IsKernelStarted(This,pVal)

#define IDeviceControl_get_Offset(This,pVal)	\
    (This)->lpVtbl -> get_Offset(This,pVal)

#define IDeviceControl_put_Offset(This,newVal)	\
    (This)->lpVtbl -> put_Offset(This,newVal)

#define IDeviceControl_get_AccessSize(This,pVal)	\
    (This)->lpVtbl -> get_AccessSize(This,pVal)

#define IDeviceControl_put_AccessSize(This,newVal)	\
    (This)->lpVtbl -> put_AccessSize(This,newVal)

#define IDeviceControl_get_Value(This,pVal)	\
    (This)->lpVtbl -> get_Value(This,pVal)

#define IDeviceControl_put_Value(This,newVal)	\
    (This)->lpVtbl -> put_Value(This,newVal)

#define IDeviceControl_get_DeviceName(This,pVal)	\
    (This)->lpVtbl -> get_DeviceName(This,pVal)

#define IDeviceControl_put_DeviceName(This,newVal)	\
    (This)->lpVtbl -> put_DeviceName(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDeviceControl_Connect_Proxy( 
    IDeviceControl * This);


void __RPC_STUB IDeviceControl_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDeviceControl_Close_Proxy( 
    IDeviceControl * This);


void __RPC_STUB IDeviceControl_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeviceControl_get_Valid_Proxy( 
    IDeviceControl * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IDeviceControl_get_Valid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDeviceControl_RaiseInterrupt_Proxy( 
    IDeviceControl * This,
    /* [in] */ long inhno);


void __RPC_STUB IDeviceControl_RaiseInterrupt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDeviceControl_Map_Proxy( 
    IDeviceControl * This,
    /* [in] */ long address,
    /* [in] */ long size);


void __RPC_STUB IDeviceControl_Map_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDeviceControl_Unmap_Proxy( 
    IDeviceControl * This,
    /* [in] */ long address);


void __RPC_STUB IDeviceControl_Unmap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeviceControl_get_IsKernelStarted_Proxy( 
    IDeviceControl * This,
    /* [retval][out] */ BOOL *pVal);


void __RPC_STUB IDeviceControl_get_IsKernelStarted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeviceControl_get_Offset_Proxy( 
    IDeviceControl * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IDeviceControl_get_Offset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeviceControl_put_Offset_Proxy( 
    IDeviceControl * This,
    /* [in] */ long newVal);


void __RPC_STUB IDeviceControl_put_Offset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeviceControl_get_AccessSize_Proxy( 
    IDeviceControl * This,
    /* [retval][out] */ short *pVal);


void __RPC_STUB IDeviceControl_get_AccessSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeviceControl_put_AccessSize_Proxy( 
    IDeviceControl * This,
    /* [in] */ short newVal);


void __RPC_STUB IDeviceControl_put_AccessSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeviceControl_get_Value_Proxy( 
    IDeviceControl * This,
    /* [retval][out] */ long *pVal);


void __RPC_STUB IDeviceControl_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeviceControl_put_Value_Proxy( 
    IDeviceControl * This,
    /* [in] */ long newVal);


void __RPC_STUB IDeviceControl_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDeviceControl_get_DeviceName_Proxy( 
    IDeviceControl * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IDeviceControl_get_DeviceName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDeviceControl_put_DeviceName_Proxy( 
    IDeviceControl * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IDeviceControl_put_DeviceName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeviceControl_INTERFACE_DEFINED__ */



#ifndef __ATLDEVICELib_LIBRARY_DEFINED__
#define __ATLDEVICELib_LIBRARY_DEFINED__

/* library ATLDEVICELib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ATLDEVICELib;

#ifndef ___IDeviceControlEvents_DISPINTERFACE_DEFINED__
#define ___IDeviceControlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IDeviceControlEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IDeviceControlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("49253060-4210-43A1-8EA2-3A97587C89B9")
    _IDeviceControlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IDeviceControlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IDeviceControlEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IDeviceControlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IDeviceControlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IDeviceControlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IDeviceControlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IDeviceControlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IDeviceControlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IDeviceControlEventsVtbl;

    interface _IDeviceControlEvents
    {
        CONST_VTBL struct _IDeviceControlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IDeviceControlEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IDeviceControlEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IDeviceControlEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IDeviceControlEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IDeviceControlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IDeviceControlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IDeviceControlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IDeviceControlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_DeviceControl;

#ifdef __cplusplus

class DECLSPEC_UUID("D6626115-E35B-4B38-BC65-3E59D5535AD4")
DeviceControl;
#endif
#endif /* __ATLDEVICELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


