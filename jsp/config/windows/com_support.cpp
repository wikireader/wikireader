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
 *  @(#) $Id: com_support.cpp,v 1.10 2003/12/15 07:19:22 takayuki Exp $
 */

#include <vitron.h>
#include <com_support.h>
#include <eventlog.h>
#include "syslog.h"

#include <resource.h>

#include <commctrl.h>

/*
 *  前に聞かれた事の説明
 *     Q: CスタイルコメントとC++スタイルコメントが混ざっていますが?
 *     A: 気楽にコメントアウトされると困る処理がCスタイルコメントで、似たような処理をする場所の説明にC++スタイルコメントを使っています。
 *
 *     Q: 変数の名づけ規則がめちゃくちゃですが?
 *     A: 基本的にスコープ範囲で使い分けています。
 *          ローカルが小文字+"_", グローバルが大文字小文字, メンバがJavaっぽいスタイル
 *
 *     Q: INVALID_HANDLE_VALUE と NULL をどう使い分けているのですか?
 *     A: CreateXxxxが失敗時に返却する値であわせました。
 *        具体的には、ファイルはINVALID_HANDLE_VALUE(CreateFile)で、それ以外がNULLになってます。
 */

#ifndef __COM_NOT_REQUIRED__


//===================================================
/*
 *   共通部
 */
extern "C" SYSTIM		_kernel_systim_offset;
extern "C" SYSTIM		_kernel_current_time;
extern "C" HINSTANCE	ProcessInstance;
extern "C" HANDLE		PrimaryDialogHandle;
extern "C" BOOL			ShutdownPostponementRequest;
extern "C" int          CPUStatus;

extern "C" void FatalAssertion(int exp, LPCSTR format, ... );

extern "C" void enter_system_critical_section(BOOL * cookie);
extern "C" void leave_system_critiacl_section(BOOL * cookie);

#ifdef KERNEL_DEBUG_MODE
   extern "C" _kernel_debugprintf(const char * format, ... );
#  define kprintf(x) _kernel_debugprintf x
#else
#  define kprintf(x)
#endif

//===================================================

	/* ユーザ定義のログ */
#define USERDEFINED	0x60
#define LOG_MODULENAME (0 | USERDEFINED)	/* モジュールの名前   */
#define LOG_TIMESTAMP  (1 | USERDEFINED)    /* 記録を開始した時刻 (SYSTEMTIME構造体をそのままダンプ) */

#define  __HAL_MSG_MSGONLY
#include <hal_msg.h>

#include <string>

namespace
{
	/*
	 *   COMインターフェース関連の定義
	 */

		//諸々のIDの宣言
	const IID   IID_IKernelEvents = {0x1353969D,0xE84F,0x463F,{0xB2,0x11,0x33,0x7E,0x9B,0xCF,0xB9,0x9E}};
	const IID   IID_IKernel       = {0xD3E42099,0x3FDD,0x4A78,{0xBD,0xBD,0x4E,0x57,0xD3,0x62,0xF5,0xED}};
	const CLSID CLSID_Kernel      = {0x51789667,0x9F20,0x40AF,{0xAF,0x7F,0x98,0x56,0x32,0x5D,0xFB,0x0B}};

		// IKernelインターフェース定義
	class IKernel : public IUnknown
	{
	public:
		STDMETHOD(Read)(unsigned long address,unsigned long sz,char __RPC_FAR data[]) PURE;
		STDMETHOD(IsValid)(BOOL __RPC_FAR * valid) PURE;
		STDMETHOD(Write)(unsigned long address,unsigned long sz,char __RPC_FAR data[]) PURE;
		STDMETHOD(OnLogEvent)(long sz, unsigned char __RPC_FAR data[]) PURE;
	};

		// IKernelEventsインタフェース定義
	class IKernelEvents : public IUnknown
	{
	public:
		STDMETHOD(QueryInterface)(REFIID iid, void ** unk);
		STDMETHOD_(ULONG,AddRef)();
		STDMETHOD_(ULONG,Release)();
		STDMETHOD(OnInterruptRequest)(long inhno);

		IKernelEvents(void);

		long RefCount;
	};

	/*
	 *   IKernelEvents実装
	 */

	IKernelEvents::IKernelEvents(void) : RefCount(0)
	{}

    STDMETHODIMP IKernelEvents::QueryInterface(REFIID iid, void ** unk)
	{
		if( iid == IID_IKernelEvents || iid == IID_IUnknown )
		{
			*unk = this;
			::InterlockedIncrement(&RefCount);
			return S_OK;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) IKernelEvents::AddRef()
	{	return ::InterlockedIncrement(&RefCount);	}

	STDMETHODIMP_(ULONG) IKernelEvents::Release()
	{
		if( ::InterlockedDecrement(&RefCount) == 0)
			delete this;
		return RefCount;
	}

	STDMETHODIMP IKernelEvents::OnInterruptRequest(long inhno)
	{

		if(inhno < 0)
		{
			/* inhno < 0 は特殊な意味を持つ */

			/* inhno = -1 : keep-alive */
		}else
			::PostMessage((HWND)PrimaryDialogHandle,HALMSG_MESSAGE,HALMSG_INTERRUPT,(LPARAM)inhno);
		return S_OK;
	}
}

namespace {
	/*
	 *   COM通信用作業スレッドに関する定義
	 */

	HANDLE request_semaphore    = NULL;		//要求待ちセマフォ
	HANDLE worker_thread_handle = NULL;		//作業スレッドのハンドル

	//==============================================================================

		//COM作業スレッドが正常に作動しているか否か
	inline bool IsValid(void)
	{	return (request_semaphore != NULL) && (worker_thread_handle != NULL);	}

	//===================================================

		/*
		 *    Q: どうしてシステムロックが必要なの?
		 *    A: ログイベントをキューイングするために各タスク/割込みスレッドがメモリを取る
		 *        -> VisualC++のマルチスレッド用メモリ確保ライブラリは内部でクリティカルセクションオブジェクトを使って排他制御している
		 *        -> TOPPERS/JSP Windowsのディスパッチャ(管理スレッド)は要求が来ると今動いているスレッドを止めにかかる
		 *        -> メモリ確保用のクリティカルセクションを持ったままSuspendThreadされてしまい、メモリ確保要求を出した他のスレッド全てがロック
		 *        -> 管理スレッドもログを出すためにメモリをとりに来るので、管理スレッドもロック
		 *        -> 管理スレッドがロックされると、最初にSuspendThreadされたヤツを起こすスレッドがいない
		 *        -> デッドロック
		 *    補足 : COM通信作業スレッドやコンソール管理スレッドのように管理スレッドのSuspendThreadの対象にならないスレッドはロックしなくてもいい
		 */

	//===================================================

		/* 作業スレッドに与えるデータを保持するクラス */
	class Request
	{
	public:
		enum tagRequestType
		{
			Confirmation = 0x00,	/* ワーカスレッドの動作確認 (セマフォをたたくだけ) */
			
			DeviceRead   = 0x01,	/* デバイスエミュレーション 読出し操作 */
			DeviceWrite  = 0x02,	/* デバイスエミュレーション 書込み操作 */

			EventLog     = 0x03,	/* イベントログ送出 */

			QuitThread   = 0xff		/* 作業スレッドの終了 */
		};

	protected:
		bool                blocking;
		bool				allocated;
		enum tagRequestType type;
		unsigned long       address;		/* デバイス : デバイスのあるアドレス */
		unsigned long       size;			/* 共通     : storageが持つデータのサイズ */
		HGLOBAL             storage;		/* 共通     : データを格納している領域のアドレス(ハンドル) */
		int                 result;			/* 共通     : ブロッキング実行時の返却地 */
		HANDLE              signalobject;	/* 共通     : ブロッキング実行時のスレッドハンドル */

		Request *           next;		/* キューにするためのリンク */

		static LONG      RequestCount;	//キューにつながっているリクエストの数
		static Request * top;			//リクエストキューの先頭 (もっとも古いリクエスト)
		static Request * tail;			//リクエストキューの末尾 (もっとも新しいリクエスト)
		static CRITICAL_SECTION cs;		//排他キュー操作用危険領域オブジェクト

		void connect(void)
		{
				//要求をキューにつなぐ
			::EnterCriticalSection(&cs);
			next = NULL;
			if(tail != NULL)
				tail->next = this;
			tail = this;
			if(top == NULL)
				top = tail;
			::LeaveCriticalSection(&cs);

			::InterlockedIncrement(&RequestCount);
		}

	protected:
			//コンストラクタ
		Request(enum tagRequestType _type, bool _block, unsigned int _size = 0, void * _storage = NULL, unsigned long _address = 0)
            :   blocking(_block), allocated(false), type(_type), address(_address), 
                size(0), storage(NULL), result(-1), signalobject(NULL), next(NULL)
		{
			size    = _size;

			if(size != 0)
			{
					//領域が指定されてない場合、動的に確保する
				if(_storage != NULL)
					storage = static_cast<HGLOBAL>(_storage);
				else
					allocate(size, false);
			}
		}

	public:
			//デストラクタ
		virtual ~Request(void)
		{

				//領域が動的確保されていた場合、破棄を行う
			if(storage != NULL && allocated)
			{
				BOOL lock;

				enter_system_critical_section(&lock);
	            ::GlobalFree(storage);
				leave_system_critiacl_section(&lock);
			}
		}

		static void initialize(void)
		{
            kprintf(("Request::initialize()\n"));
			if(RequestCount == -1)
			{
				::InitializeCriticalSection(&cs);
				RequestCount = 0;
				top  = NULL;
				tail = NULL;
			}
		}

		static void finalize(void)
		{
            kprintf(("Request::finalize()\n"));
			if(RequestCount != -1)
			{
				while(top != NULL)
				{
					Request * target = top;
					top = top->next;
					delete target;
				}

				::DeleteCriticalSection(&cs);	
				RequestCount = -1;
			}
		}

			// システムをロックしないでメモリ開放
		inline void release(void)
		{
			if(storage != NULL && allocated)
			{
				::GlobalFree(storage);
				storage = 0;
			}
		}

		inline bool isBlockingRequest(void) const
		{	return blocking;	}

		inline enum tagRequestType getType(void) const
		{	return type;	}

		inline bool operator == (enum tagRequestType _type) const
		{	return type == _type;	}

		inline bool operator != (enum tagRequestType _type) const
		{	return !(operator ==(_type));	}

		inline void * getStorage(void)
		{	return reinterpret_cast<void *>(storage);	}

		inline unsigned long getAddress(void) const
		{	return address;   }

		inline unsigned long getSize(void) const
		{	return size;	}

		inline int getResult(void) const
		{	return result;	}

		static inline LONG getRequestCount(void)
		{	return RequestCount;	}

			//領域の確保
			//	_size   : 必要とする領域のサイズ
			//	realloc : 内容を保持するか否か
		bool allocate(unsigned int _size, bool realloc = false)
		{
			unsigned long old_size;
			HGLOBAL       old_storage;
			BOOL          lock;

			FatalAssertion(type != Confirmation, "Request::allocate was performed with Confirmation Request Object.");

			old_size    = size;
			old_storage = storage;

			enter_system_critical_section(&lock);
			storage = ::GlobalAlloc(GMEM_FIXED, _size);

			FatalAssertion(storage != NULL, "Request::allocate failed to allocate a memory block.");

				//前の内容の保持 および領域破棄
			if(old_storage != NULL)
			{
				if(realloc)
					::CopyMemory(storage, old_storage, old_size);

				if(allocated)
					::GlobalFree(old_storage);
			}
			leave_system_critiacl_section(&lock);

			allocated = true;
			size = _size;

			return true;
		}

			//内容を保持したまま再アロケート
		inline bool reallocate(unsigned int _size)
		{	return allocate(_size, true);   }

			//保持領域に対する書込み
		inline void set(void * data, unsigned int sz, unsigned int offset = 0)
		{
			if(size < sz)
				reallocate(sz + offset);

			::CopyMemory(reinterpret_cast<char *>(storage) + offset, data, sz);
		}

			//保持領域に対する読込み
		inline bool get(void * data, unsigned int sz, unsigned int offset = 0)
		{
			if(size < sz + offset)
				return false;

			::CopyMemory(data, reinterpret_cast<char *>(storage) + offset, sz);
		}

			//保持領域の任意の位置に対するポインタ取得
			//		offset : 先頭からのバイトオフセット
		void * getptr(unsigned int offset = 0)
		{
			if(offset > size || storage == NULL)
				return NULL;

			return reinterpret_cast<void *>(reinterpret_cast<char *>(storage) + offset);
		}

			/*
			 *   Requestオブジェクトファクトリ
			 *     Q: どうしてファクトリが必要なのか?
			 *     A: メモリ確保よりも先にCPUをロックする必要があるため (詳細は上記CPUロックする理由を参照)
			 */			
		static Request * Create(enum tagRequestType _type, bool _block, unsigned int _size = 0, void * _storage = NULL, unsigned long _address = 0)
		{
			Request * request;
			BOOL      lock;

			enter_system_critical_section(&lock);
			request = new Request(_type, _block, _size, _storage, _address);
			leave_system_critiacl_section(&lock);
		
			return request;
		}

		static Request * GetRequest(void)
		{
			Request * result;

			::EnterCriticalSection(&cs);
			result = top;
			if(top != NULL)
			{
				top = top->next;
				if(top == NULL)
					tail = NULL;
			}
			::LeaveCriticalSection(&cs);

			::InterlockedDecrement(&RequestCount);

			return result;
		}

		void Finalize(bool succeeded)
		{
			if(this != 0) {
				if(isBlockingRequest())
					signal(succeeded ? getSize() : -1);
				else
				{
					BOOL lock;

					enter_system_critical_section(&lock);
					delete this;
					leave_system_critiacl_section(&lock);
				}
			}
		}

        virtual void signal(int _result = 0)
		{
			result = _result;
			if(signalobject != NULL)
				::PostThreadMessage((DWORD)signalobject, WM_QUIT, 0, 0);
		}
				
			/*
			 *   新しい要求の発行
			 *     (この関数を使うスレッド : 管理スレッド, 割込みスレッド, タスクスレッド)
			 */
		virtual int invoke(void)
		{
			int  _result;
			BOOL lock;
			MSG  msg;

			_result = 0;

			if(!IsValid())
				return -1;

			enter_system_critical_section(&lock);

			connect();

				//要求の実行待ち
			if(blocking)
			{
				signalobject = (HANDLE)::GetCurrentThreadId();

					//新しい要求をだしたことを通知
				::ReleaseSemaphore(request_semaphore,1,NULL);
				leave_system_critiacl_section(&lock);

					//WM_QUITメッセージを使って同期を取る
				while(::GetMessage(&msg, 0, 0, 0) != 0)
					::DispatchMessage(&msg);

					//このオブジェクトを破棄するのでバックアップを取る
				_result = result;

					//キューからはずす処理は作業スレッドがやる
					//delete requestは、ノンブロッキングなら作業スレッド責任、ブロッキングならここでやる
                enter_system_critical_section(&lock);
				delete this;
                leave_system_critiacl_section(&lock);
			}else
			{
					//新しい要求をだしたことを通知
				::ReleaseSemaphore(request_semaphore,1,NULL);
				leave_system_critiacl_section(&lock);
			}

			return _result;
		}
	};

	class EventDumpRequest : public Request
	{
	protected:
		static HANDLE FileHandle;

		EventDumpRequest(enum tagRequestType _type, bool _block, unsigned int _size = 0, void * _storage = NULL, unsigned long _address = 0) : Request(_type, _block, _size, _storage, _address) 
		{}

	public:
		static void initialize(void)
		{
			SYSTEMTIME systim;
			DWORD      written;
			char       buffer[1024];
			char *     top;
			char *     work;

			::GetLocalTime(&systim);
			::wsprintf(buffer, "kernel-log-%04d%02d%02d-%02d%02d%02d.log", systim.wYear, systim.wMonth, systim.wDay, systim.wHour, systim.wMinute, systim.wSecond);
			FileHandle = ::CreateFile(buffer, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN, NULL);

            kprintf(("EventDumpRequest : initialize (filename = '%s')\n", buffer));

			T_RGLOG_HEADER log;
			
			log.logtype = LOG_TIMESTAMP;
			log.logtim  = 0;
			log.bufsz   = sizeof(SYSTEMTIME);
			log.valid   = 1;
			::WriteFile(FileHandle, &log, sizeof(log), &written, NULL);
			::WriteFile(FileHandle, &systim, sizeof(SYSTEMTIME), &written, NULL);

				/* モジュール名を取り出す */
			::lstrcpyn(buffer, ::GetCommandLine(), 1023);
			top = buffer;
			buffer[1023] = '\x0';	//サーチ用番兵

			if(*top == '"')
			{
				++ top;
				for(work = top; *work != '\x0' && *work != '"'; ++work);
			}
			else
				for(work = top; *work != '\x0' && *work != ' '; ++work);

			*work = '\x0';	//ターミネート処理

			log.logtype = LOG_MODULENAME;
			log.logtim  = 0;
			log.bufsz   = work - top + 1;	//ターミネータの分
			log.valid   = 1;
			::WriteFile(FileHandle, &log, sizeof(log), &written, NULL);
			::WriteFile(FileHandle, top, log.bufsz, &written, NULL);
		}

		static void finalize(void)
		{
            kprintf(("EventDumpRequest : finalize()\n"));

			if(FileHandle != INVALID_HANDLE_VALUE)      //CreateFile returns INVALID_HANDLE_VALUE if it failed
				::CloseHandle(FileHandle);
		}

		static EventDumpRequest * Create(enum tagRequestType _type, bool _block, unsigned int _size = 0, void * _storage = NULL, unsigned long _address = 0)
		{
			EventDumpRequest * request;
			BOOL      lock;

			enter_system_critical_section(&lock);
			request = new EventDumpRequest(_type, _block, _size, _storage, _address);
			leave_system_critiacl_section(&lock);
		
			return request;
		}

		virtual int invoke(void)
		{
			DWORD written;
			BOOL  lock;

			enter_system_critical_section(&lock);
			if(FileHandle != INVALID_HANDLE_VALUE)      //CreateFile return INVALID_HANDLE_VALUE, not NULL, if it failed its operatation.
				::WriteFile(FileHandle, storage, size, &written, NULL);
			delete this;
			leave_system_critiacl_section(&lock);

			return 0;
		}
	};

    class ExclusiveDeviceRequest : public Request
    {
    protected:
        static HANDLE blocker;
    public:

        static void initialize(void) throw()
        {
            kprintf(("ExclusiveDeviceRequest::initialize()\n"));

            if(blocker == NULL) {
                blocker = ::CreateEvent(NULL, FALSE, FALSE, NULL);
            }
        }

        static void finalize(void) throw()
        {
            kprintf(("ExclusiveDeviceRequest::finalize()\n"));

            if(blocker != NULL) {
                ::CloseHandle(blocker);
            }
        }

        virtual void signal(int _result = 0)
        {
            result = _result;
            ::SetEvent(blocker);
        }

        virtual int invoke(void)
        {
            int  _result = 0;
            BOOL lock;
            BOOL cpustate;

                //割り込みを禁止する
            if((cpustate = CPUStatus) == 0)
                dis_int(0);

            enter_system_critical_section(&lock);

                //要求をキューの先頭につなぐ
            ::EnterCriticalSection(&cs);
            next = top;
            top = this;
            if(tail == NULL)
                tail = this;
            ::LeaveCriticalSection(&cs);
            ::InterlockedIncrement(&RequestCount);

            ::ReleaseSemaphore(request_semaphore, 1, NULL);
            leave_system_critiacl_section(&lock);

            if(blocking) {
                ::WaitForSingleObject(blocker, INFINITE);

                _result = result;

                enter_system_critical_section(&lock);
                delete this;
                leave_system_critiacl_section(&lock);
            }

            if(cpustate == 0)
                ena_int(0);

            return _result;
        }
    };

//	typedef class EventDumpRequest EventRequest;
	typedef class Request EventRequest;		//イベントログ出力要求で使用されるリクエスト
//	typedef class Request DeviceRequest;	//デバイス通信要求で使用されるリクエスト
	typedef class ExclusiveDeviceRequest DeviceRequest;	//デバイス通信要求で使用されるリクエスト

	//==============================================================================

	LONG				Request::RequestCount = -1;	//キューにつながっているリクエストの数
	Request *			Request::top  = NULL;		//リクエストキューの先頭 (もっとも古いリクエスト)
	Request *			Request::tail = NULL;		//リクエストキューの末尾 (もっとも新しいリクエスト)
	CRITICAL_SECTION	Request::cs;				//排他キュー操作用危険領域オブジェクト

	HANDLE              EventDumpRequest::FileHandle    = INVALID_HANDLE_VALUE;     //CreateFile uses INVALID_HANDLE_VALUE for identifying an invalid instance 
    HANDLE              ExclusiveDeviceRequest::blocker = NULL;

	//==============================================================================

		/*
		 *   作業スレッドが使用する作業関数の定義
		 */

		//コネクションポイントへの接続
	bool Advise(IUnknown * container, REFIID iid, IUnknown * sink, DWORD * cookie, IConnectionPoint ** p_cp = NULL)
	{
		IConnectionPointContainer * cp_container;
		IConnectionPoint * cp;

        if(container == 0 || sink == 0 || cookie == 0)
            return false;

		if(p_cp == 0)
			p_cp = &cp;

			//IUnknownインタフェースからIConnectionPointContainerへキャスト
		container->QueryInterface(IID_IConnectionPointContainer, (void **)&cp_container);
		if(cp_container == 0)
			return false;

			//ConnectionPointContainerから要求に合うConnectionPointを探す
		cp_container->FindConnectionPoint(iid, p_cp);
		cp_container->Release();
		if(*p_cp == 0)
			return false;

			//コネクションポイント接続
		if(FAILED((*p_cp)->Advise(sink, cookie)))
			return false;

		if(p_cp == &cp)
			cp->Release();
		return true;
	}

	//==============================================================================
	
    /*
     *  PerformRequest : 要求の実行
     *     返却値 : イベント処理を継続する(true) or 作業スレッドを停止させる(false)
     */

    bool PerformRequest(IKernel * manager)
    {
        Request * request;
        bool succeeded;

			//キューからリクエストを抜き取る
		request = Request::GetRequest();
		FatalAssertion(request != NULL,"");

			//終了要求
		if(request->getType() == Request::QuitThread)
		{
			if(!request->isBlockingRequest())
			{
				BOOL lock;

				enter_system_critical_section(&lock);
				delete request;
				leave_system_critiacl_section(&lock);
			}
            return false;
		}

			//要求毎の処理
		switch(request->getType())
		{
		case Request::Confirmation:
			succeeded = true;
			break;

		case Request::DeviceRead:
			succeeded = SUCCEEDED(manager->Read(request->getAddress(), request->getSize(), reinterpret_cast<char *>(request->getStorage())));
			break;

		case Request::DeviceWrite:
			succeeded = SUCCEEDED(manager->Write(request->getAddress(), request->getSize(), reinterpret_cast<char *>(request->getStorage())));
			break;

		case Request::EventLog:
			succeeded = SUCCEEDED(manager->OnLogEvent(request->getSize(), reinterpret_cast<unsigned char *>(request->getStorage())));

				//シャットダウン中ならログを全部はき終るまで待ってもらう
			ShutdownPostponementRequest = TRUE;
			break;

		default:
			succeeded = false;
		}

        request->Finalize(succeeded);

        return true;
    }

		/*
		 *   COM通信を行うための作業スレッド 本体
		 */
	DWORD WINAPI WorkerThreadProcess(LPVOID _param)
	{
		IKernel * manager;
		IKernelEvents * sink;
		IConnectionPoint * cp;
		DWORD cookie;
		BOOL Success;

        kprintf(("WorkerThreadProcess : start\n"));

			/* 初期化 */
		::CoInitialize(NULL);

			// デバイスマネージャと接続
		manager = 0;
		if(FAILED(::CoCreateInstance(CLSID_Kernel, NULL, CLSCTX_ALL, IID_IKernel, (void **)&manager)))
			goto _WorkerThreadProc_Cleanup;

		cp = 0;
        sink = new(std::nothrow) IKernelEvents;
		if(!Advise(manager, IID_IKernelEvents, sink, &cookie,&cp))
		{
            if(sink != 0)
    			delete sink;
			goto _WorkerThreadProc_Cleanup;
		}

			/* メインルーチン */
		manager->IsValid(&Success);
		if(Success != 0)
		{
            MSG msg;
			DWORD work;
            bool  loop_flag = true;
			
			do {
                    /*
                     * 外部からCOMをたたくとメッセージとして飛んでくるため、WaitForSingleObjectでセマフォを待つと外部からの要求を実行できなくなる。
                     * そのため、MsgWaitFor...を用い、メッセージが飛んできた場合はこれに対処できるようにする。
                     */

                    /*
                     * cygwin環境だとQS_ALLPOSTMESSAGEが定義されないので、これで対処
                     */
#ifndef QS_ALLPOSTMESSAGE
#define QS_ALLPOSTMESSAGE (0x0100)
#endif

                    //要求待ち
                work = ::MsgWaitForMultipleObjects(1, &request_semaphore, FALSE, INFINITE, QS_ALLPOSTMESSAGE);

                switch(work) {

                    //処理要求の実行
                case WAIT_OBJECT_0:
                        loop_flag = PerformRequest(manager);
                        break;

                    //外部からの要求 (割込みなど)
                case WAIT_OBJECT_0 + 1:
                        GetMessage(&msg, 0, 0, 0);
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                        break;
                
                default:
                    loop_flag = false;
                }

			} while(loop_flag);
		}

			/*
			 * Finalize
			 */

		if(cp != 0)
		{
			cp->Unadvise(cookie);
			cp->Release();
			cp = 0;
			sink = 0;
		}

	_WorkerThreadProc_Cleanup:

        if(manager != 0)
			manager->Release();

		CloseHandle(worker_thread_handle);
		worker_thread_handle = NULL;
        manager = 0l;
		::CoUninitialize();

		/* キューを空にする (待ち解除) */

		Request * request;
		while((request = Request::GetRequest()) != 0)
			request->Finalize(false);
		
        kprintf(("WorkerThreadProcess : exit\n"));

        return 0;
	}
}

//==============================================================================

/****************************************************
 *                                                  *
 * TOPPERS/JSP - Windowsシミュレーション環境        *
 *   COMを用いたデバイスエミュレーション機能        *
 *                                                  *
 ****************************************************/

#ifdef DEVICE_ENABLE

extern "C"
int _cdecl DeviceRead(unsigned long address, unsigned long size, void * storage)
{
	if(!IsValid() || storage == NULL)
		return -1;

	return DeviceRequest::Create(Request::DeviceRead, true, size, storage, address)->invoke();
}

extern "C"
int _cdecl DeviceWrite(unsigned long address, unsigned long size, void * storage)
{
	if(!IsValid() || storage == NULL)
		return -1;

	return DeviceRequest::Create(Request::DeviceWrite, true, size, storage, address)->invoke();
}

#endif


/****************************************************
 *                                                  *
 * TOPPERS/JSP - Windowsシミュレーション環境        *
 *   COMを用いたカーネルログ取得機能                *
 *                                                  *
 ****************************************************/

#ifdef EVENTLOG_ENABLE

	/* 多重ログ出力防止用セマフォ :  1を足した結果が1であったときだけ、ログを出力できる */
static LONG event_write_semaphore = 0;

void event_write(unsigned int logtype, unsigned int valid, UINT bufsz, ... )
{
	EventRequest * event_request;
	DBIFLOG * log;
	va_list   vl;

	if(IsValid() && TlsGetValue(TLS_LOGMASK) == 0)
	{
			// リクエスト領域確保
		event_request = EventRequest::Create(EventRequest::EventLog, false, sizeof(T_RGLOG_HEADER) + bufsz);
		log     = reinterpret_cast<DBIFLOG *>(event_request->getptr());

			// ヘッダ生成
		log->header.logtype = logtype;
		log->header.logtim  = _kernel_systim_offset + _kernel_current_time;
		log->header.valid   = valid;
		log->header.bufsz   = bufsz;

			// 本体の格納
		va_start(vl, bufsz);
		for(unsigned int i=0;i<bufsz/sizeof(int);i++)
			*((int *)&log->body + i) = va_arg(vl, int);

			// 要求の送付 (Non-blocking)
		event_request->invoke();
	}
}

extern "C"
void event_write_svc_enter(int fncd, unsigned int params, ... )
{
	EventRequest * event_request;
	DBIFLOG *      log;
	va_list        vl;
	unsigned int   i;

	if(IsValid() && TlsGetValue(TLS_LOGMASK) == 0)
	{
			//領域確保 : 確保するサイズはパラメータ総数より一個分多い (呼び出し元アドレスを格納)
		event_request = EventRequest::Create(EventRequest::EventLog, false, sizeof(T_RGLOG_HEADER) + sizeof(T_RGLOG_SVC) + params * sizeof(VP_INT));	
		log     = reinterpret_cast<DBIFLOG *>(event_request->getptr());

			//パケット生成
		log->header.logtype = LOG_TYPE_SVC;
		log->header.logtim  = _kernel_systim_offset + _kernel_current_time;
		log->header.valid   = 0x7fffffff >> (29 - params);
		log->header.bufsz   = (params+3) * sizeof(int);

		va_start(vl, params);

		log->body.svc.fncno  = fncd;
		log->body.svc.prmcnt = params;

			/* 一回ループが多いのは最後に呼出元アドレスを積んでいるため */
		for(i=0;i<params+1;i++)
			log->body.svc.prmary[i] = va_arg(vl, VP_INT);

		event_request->invoke();
	}
}

extern "C"
void event_write_svc_leave(int fncd, unsigned int ercd, unsigned int retaddr)
{
	EventRequest * event_request;
	DBIFLOG * log;

	if(IsValid() && TlsGetValue(TLS_LOGMASK) == 0)
	{
			//領域確保 : 確保するサイズはパラメータ総数(返却値のみなので１つ)より一個分多い (呼び出し元アドレスを格納)
		event_request = EventRequest::Create(EventRequest::EventLog, false, sizeof(T_RGLOG_HEADER) + sizeof(T_RGLOG_SVC) + sizeof(VP_INT));
		log     = reinterpret_cast<DBIFLOG *>(event_request->getptr());

			//ヘッダ部生成
		log->header.logtype = LOG_TYPE_SVC|LOG_LEAVE;
		log->header.logtim  = _kernel_systim_offset + _kernel_current_time;
		log->header.valid   = 0x7;
		log->header.bufsz   = 4 * sizeof(int);

			//ボディ部生成
		log->body.svc.fncno     = fncd;
		log->body.svc.prmcnt    = 1;
		log->body.svc.prmary[0] = (VP_INT)ercd;
		log->body.svc.prmary[1] = (VP_INT)retaddr;

		event_request->invoke();
	}
}

extern "C" void decode_taskstatus(void * tcb, int * tskid, unsigned int * tskstat, unsigned int * objtype, int * objid);

extern "C" 
void event_write_tskstat(void * tcb)
{
	EventRequest * event_request;
	DBIFLOG * log;

	if(IsValid())	//これはカーネル操作なのでTLS_LOGMASKを見てはいけない
	{
			//領域確保
		event_request = EventRequest::Create(EventRequest::EventLog, false, sizeof(T_RGLOG_HEADER) + sizeof(T_RGLOG_TSKSTAT));
		log     = reinterpret_cast<DBIFLOG *>(event_request->getptr());

		log->header.logtype = LOG_TYPE_TSKSTAT;
		log->header.logtim  = _kernel_systim_offset + _kernel_current_time;
		log->header.valid   = 0xf;
		log->header.bufsz   = 4 * sizeof(int);

		decode_taskstatus(tcb, &log->body.tskstat.tskid, &log->body.tskstat.tskstat, &log->body.tskstat.tskwait, &log->body.tskstat.wobjid);

		event_request->invoke();
	}
}

#endif


static BOOL CALLBACK NotifyDialogProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			LONG  count;
			HWND  hCtrl;
			
			hCtrl = ::GetDlgItem(hDlg, IDC_PROGRESS1);
			count = Request::getRequestCount();

			::SetWindowLong(hDlg, GWL_USERDATA, count);
			::SendMessage(hCtrl, PBM_SETRANGE32, 0, count);
			::SendMessage(hCtrl, PBM_SETPOS, count - Request::getRequestCount(), 0);

			::SetTimer(hDlg, 100, 200, NULL);
			break;
		}

	case WM_TIMER:
		if(wParam == 100)
		{
			LONG  count;
			HWND  hCtrl;

			hCtrl = ::GetDlgItem(hDlg, IDC_PROGRESS1);
			count = ::GetWindowLong(hDlg, GWL_USERDATA);

			::SendMessage(hCtrl, PBM_SETPOS, count - Request::getRequestCount(), 0);

			if(Request::getRequestCount() <= 0)
				::PostQuitMessage(0);
		}
		break;
	case WM_CLOSE:
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

extern "C"
void _cdecl InitializeComSupportModule(void)
{
    kprintf(("InitializeComSupportModule()\n"));

	Request::initialize();
	DeviceRequest::initialize();
	EventRequest::initialize();


	request_semaphore = ::CreateSemaphore(0, 0, 65536, 0);
	FatalAssertion(request_semaphore != NULL, "Object Creation Error : request_semaphore");

	worker_thread_handle = ::CreateThread(0, 0, WorkerThreadProcess, 0, 0, 0);
	FatalAssertion(worker_thread_handle != NULL, "Object Creation Error : worker_thread");

	Request * request = Request::Create(Request::Confirmation, true);
	request->invoke();
}

extern "C"
void _cdecl FinalizeComSupportModule(void)
{
    kprintf(("FinalizeComSupportModule()\n"));

    if(IsValid())
	{
		HWND  hDlgitem;
		HWND  NotifyDialog;
		MSG   msg;

			//ログが溜まりすぎるとログ吐きに時間がかかり、プログラムが暴走したように見えるので
			//「ログを吐いてるから待て」と警告を出す。
		NotifyDialog = ::CreateDialog(ProcessInstance, MAKEINTRESOURCE(QUITNOTICEDIALOG), NULL, NotifyDialogProc);
		hDlgitem     = ::GetDlgItem(NotifyDialog, IDC_NOTIFYMSG);
		::SetWindowText(hDlgitem, "今 一生懸命カーネルのイベントログを出しているので、終るまで少し待ってください");
		::ShowWindow(NotifyDialog, SW_NORMAL);

		Request * request = Request::Create(Request::QuitThread, false);
		request->invoke();

			//終了ダイアログ用のメッセージポンプ

        if(NotifyDialog != NULL) {
            while(GetMessage(&msg, NULL, 0, 0) != 0) {
        	    TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

			//ちゃんとスレッドが終了するまで待機
		if(worker_thread_handle != NULL)
            ::WaitForSingleObject(worker_thread_handle, INFINITE);
		::CloseHandle(request_semaphore);
        request_semaphore = NULL;

		EventRequest::finalize();
		DeviceRequest::finalize();
		Request::finalize();

		::DestroyWindow(NotifyDialog);
	}
}


#else	// __COM_NOT_REQUIRED__

extern "C"
void _cdecl InitializeComSupportModule(void)
{}

extern "C"
void _cdecl FinalizeComSupportModule(void)
{}

#endif // __COM_NOT_REQUIRED__
