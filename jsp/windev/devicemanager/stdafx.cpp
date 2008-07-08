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
 *  @(#) $Id: stdafx.cpp,v 1.11 2003/12/24 07:40:42 takayuki Exp $
 */

/*
 * デバイスマネージャ 本体(CExeModule)
 */

#pragma warning(disable:4786)	//デバッグ文字列を255文字に切り詰めました
#pragma warning(disable:4200)	//非標準の拡張機能を使っています (長さ不定の配列:465行目)

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <atlimpl.cpp>
#include <algorithm>

#include <initguid.h>
#include "devicemanager.h"
#include "device.h"
#include "kernel.h"
#include "information.h"
#include "kernellog.h"

using namespace std;

#ifdef _DEBUG
	DWORD Count;
	char Buffer[1024];
	#define DebugOut(x) ::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE),Buffer, (x), &Count, NULL)
#else
	#define DebugOut(x) /##/
#endif

CExeModule::CExeModule(void) : Kernel(0)
{
	::InitializeCriticalSection(&cs);
}


DeviceID CExeModule::AddDevice(CDevice *device)
{
	list<CDevice *>::iterator scope;
	list<CInformation *>::iterator view;

	::EnterCriticalSection(&cs);

	scope = find(DeviceList.begin(), DeviceList.end(), device);
	if(scope != DeviceList.end())
	{
		::LeaveCriticalSection(&cs);
		return 0;
	}

	DebugOut(::wsprintfA(Buffer,"AddDevice\n"));
	view = ViewerList.begin();
	while(view != ViewerList.end())
	{
		(*view)->Fire_OnDeviceChanged(1,reinterpret_cast<long>(device));
		view ++;
	}

	DeviceList.push_back(device);

	::LeaveCriticalSection(&cs);
	return reinterpret_cast<DeviceID>(device);
}

bool CExeModule::DeleteDevice(CDevice *device)
{
	list<CDevice *>::iterator dev;
	map<Range,CDevice *>::iterator mem[2];

	::EnterCriticalSection(&cs);

	DebugOut(::wsprintfA(Buffer,"DelDevice\n"));

	dev = find(DeviceList.begin(), DeviceList.end(), device);
	if(dev == DeviceList.end())
	{
		::LeaveCriticalSection(&cs);
		return false;
	}

	mem[0] = DeviceMap.begin();
	while(mem[0] != DeviceMap.end())
	{
		if((*mem[0]).second == device)
		{
			mem[1] = mem[0];
			mem[0] ++;

			DeviceMap.erase(mem[1]);
		}else
			mem[0] ++;
	}

	DeviceList.erase(dev);

	list<CInformation *>::iterator view;
	view = ViewerList.begin();
	while(view != ViewerList.end())
	{
		(*view)->Fire_OnDeviceChanged(2,reinterpret_cast<long>(device));
		view ++;
	}

	::LeaveCriticalSection(&cs);
	return true;
}

bool CExeModule::Map(CDevice *device, Range area)
{
	map<Range,CDevice *>::iterator scope;

	::EnterCriticalSection(&cs);
	

	scope = DeviceMap.begin();
	while(scope != DeviceMap.end())
	{
		if( area.IsIncludedIn((*scope).first) )
		{
			::LeaveCriticalSection(&cs);
			return false;
		}
		scope ++;
	}

	DeviceMap[area] = device;

	DebugOut(::wsprintfA(Buffer,"Map\n"));
	list<CInformation *>::iterator view;
	view = ViewerList.begin();
	while(view != ViewerList.end())
	{
		(*view)->Fire_OnDeviceChanged(3,reinterpret_cast<long>(device),area.GetBase());
		view ++;
	}

	::LeaveCriticalSection(&cs);
	return true;
}


bool CExeModule::Unmap(CDevice *device, unsigned long address)
{
	map<Range,CDevice *>::iterator scope;
	Range work(address);

	::EnterCriticalSection(&cs);

	scope = DeviceMap.begin();
	while(scope != DeviceMap.end())
	{
		if( (*scope).second == device && work.IsIncludedIn((*scope).first) )
		{
			DeviceMap.erase(scope);

			DebugOut(::wsprintfA(Buffer,"Unmap\n"));
			list<CInformation *>::iterator view;
			view = ViewerList.begin();
			while(view != ViewerList.end())
			{
				(*view)->Fire_OnDeviceChanged(4,reinterpret_cast<long>(device),address);
				view ++;
			}

			::LeaveCriticalSection(&cs);

			return true;
		}
		scope ++;
	}
	::LeaveCriticalSection(&cs);

	return false;
}

void CExeModule::AddViewer(CInformation *view)
{
	list<CInformation *>::iterator scope;

	::EnterCriticalSection(&cs);
	scope = find(ViewerList.begin(), ViewerList.end(), view);
	if(scope == ViewerList.end())
		ViewerList.push_back(view);
	::LeaveCriticalSection(&cs);
}

void CExeModule::RemoveViewer(CInformation *view)
{
	list<CInformation *>::iterator scope;

	::EnterCriticalSection(&cs);
	scope = find(ViewerList.begin(), ViewerList.end(), view);
	if(scope != ViewerList.end())
		ViewerList.erase(scope);
	::LeaveCriticalSection(&cs);
}

bool CExeModule::AttachKernel(CKernel *kernel)
{
	bool result = false;
	list<CDevice *>::iterator scope;
	list<CInformation *>::iterator view;
	list<CKernelLog *>::iterator watcher;

	DebugOut(::wsprintfA(Buffer,"AttachKernel\n"));

	::EnterCriticalSection(&cs);
	if((Kernel != 0) && (Kernel != kernel))
	{
			//割込み-1 => カーネルのKeepAlive
		if(FAILED(Kernel->Fire_OnInterruptRequest(-1)))
		{
				//接続を失ったカーネルのデタッチ
			DetachKernel(Kernel);
		}else
		{
				//接続済みカーネルがある
			if(::MessageBox(NULL,_T("新しいカーネルが起動され、デバイスマネージャに接続しようとしています。\n新しいカーネルに接続先を切り替えますか?"),_T("TOPPERS/JSP Windows - DeviceManager"),MB_YESNO) == IDYES)
				DetachKernel(Kernel);
		}
	}

	if(Kernel == 0)
	{
		Kernel = kernel;

		scope = DeviceList.begin();
		while(scope != DeviceList.end())
		{
			(*scope)->Fire_OnKernelStart();
			scope ++;
		}

		view = ViewerList.begin();
		while(view != ViewerList.end())
		{
			(*view)->Fire_OnKernelChanged(1);
			view ++;
		}

		watcher = WatcherList.begin();
		while(watcher != WatcherList.end())
		{
			(*watcher)->Fire_OnKernelStart();
			watcher ++;
		}

		DebugOut(::wsprintfA(Buffer,"AttachKernel was performed successfully\n"));
		result = true;
	}
	::LeaveCriticalSection(&cs);

	return result;
}

bool CExeModule::DetachKernel(CKernel *kernel)
{
	bool result;
	list<CDevice *>::iterator scope;
	list<CInformation *>::iterator view;
	list<CKernelLog *>::iterator watcher;

	DebugOut(::wsprintfA(Buffer,"DetachKernel\n"));

	::EnterCriticalSection(&cs);
	if( (result = Kernel == kernel) )
	{
		scope = DeviceList.begin();
		while(scope != DeviceList.end())
		{
			(*scope)->Fire_OnKernelExit();
			scope ++;
		}

		view = ViewerList.begin();
		while(view != ViewerList.end())
		{
			(*view)->Fire_OnKernelChanged(2);
			view ++;
		}

		watcher = WatcherList.begin();
		while(watcher != WatcherList.end())
		{
			(*watcher)->Fire_OnKernelExit();
			watcher ++;
		}

		Kernel = 0l;
		DebugOut(::wsprintfA(Buffer,"DetachKernel was performed successfully\n"));
	}
	::LeaveCriticalSection(&cs);

	return result;
}

bool CExeModule::Read(CKernel * kernel, unsigned long address, unsigned long sz, unsigned char data[])
{
	bool result;
	map<Range,CDevice *>::iterator scope;

	DebugOut(::wsprintfA(Buffer,"Read\n"));

	::EnterCriticalSection(&cs);

	if(Kernel != kernel)
	{
		::LeaveCriticalSection(&cs);

		DebugOut(::wsprintfA(Buffer,"Illegal Kernel\n"));
		
		return false;
	}

	scope = DeviceMap.find(Range(address,sz));
	
	result = (scope != DeviceMap.end());
	if(result)
		(*scope).second->Fire_OnRead(address, sz, data);

	::LeaveCriticalSection(&cs);

	return result;

}

bool CExeModule::Write(CKernel *kernel, unsigned long address, unsigned long sz, unsigned char *data)
{
	bool result;
	map<Range,CDevice *>::iterator scope;

	DebugOut(::wsprintfA(Buffer,"Write\n"));

	::EnterCriticalSection(&cs);

	if(Kernel != kernel)
	{
		::LeaveCriticalSection(&cs);
		DebugOut(::wsprintfA(Buffer,"Request performed by Illegal Kernel\n"));
		return false;
	}

	scope = DeviceMap.find(Range(address,sz));
	
	result = (scope != DeviceMap.end());
	if(result)
	{
		DebugOut(::wsprintfA(Buffer,"FireWrite\n"));
		(*scope).second->Fire_OnWrite(address, sz,data);
	}
	::LeaveCriticalSection(&cs);

	return result;
}

bool CExeModule::RaiseInterrupt(unsigned long inhno)
{
	bool result = false;

	::EnterCriticalSection(&cs);

	if(Kernel != 0 && Kernel->Fire_OnInterruptRequest(inhno) >= 0)
		result = true;

	::LeaveCriticalSection(&cs);

	return result;
}

bool CExeModule::AddLogWatcher(CKernelLog * watcher)
{
	if( find(WatcherList.begin(), WatcherList.end(), watcher) != WatcherList.end() )
		return false;	//多重登録

	::EnterCriticalSection(&cs);
	WatcherList.push_back(watcher);
	::LeaveCriticalSection(&cs);

	DebugOut(::wsprintfA(Buffer,"Log watcher inserted into the queue successfully\n"));

	return true;
}


void CExeModule::RemoveLogWatcher(CKernelLog * watcher)
{
	list<CKernelLog *>::iterator scope;

	::EnterCriticalSection(&cs);
	scope = find(WatcherList.begin(), WatcherList.end(), watcher);
	if(scope != WatcherList.end())
		WatcherList.erase(scope);
	::LeaveCriticalSection(&cs);

	DebugOut(::wsprintfA(Buffer,"Log watcher removed from the queue successfully\n"));
}



/*
 *  ログ情報の種別の定義
 */
#define LOG_TYP_INTERRUPT	0x01	/* 割込みハンドラ */
#define LOG_TYP_ISR			0x02	/* 割込みサービスハンドラ */
#define LOG_TYP_TIMERHDR	0x03	/* タイムイベントハンドラ */
#define LOG_TYP_CPUEXC		0x04	/* CPU例外ハンドラ */
#define LOG_TYP_TSKEXC		0x05	/* タスク例外処理ルーチン */
#define LOG_TYP_TSKSTAT		0x06	/* タスク状態変化 */
#define LOG_TYP_DISPATCH	0x07	/* ディスパッチャ */
#define LOG_TYP_SVC			0x08	/* サービスコール */
#define LOG_TYP_COMMENT		0x09	/* コメント（文字列のみのログ） */
#define LOG_ENTER			0x00	/* 入口／開始 */
#define LOG_LEAVE			0x80	/* 出口／終了 */


typedef	unsigned int BITMASK;

typedef struct {
		unsigned int	logtype;
		unsigned int	logtim;
		BITMASK			valid;
		unsigned int	bufsz;
		unsigned char	buf[];
	} DBIFLOG;
#define DBIFLOG_BUFMARGIN (sizeof(unsigned int)*4)

bool CExeModule::OnLogEvent(CKernel *kernel, long sz, unsigned char *data)
{
	list<CKernelLog *>::iterator scope;

#if 0

	DBIFLOG * log = reinterpret_cast<DBIFLOG *>(data);
	
	DebugOut(::wsprintfA(Buffer,"LOG <%08x> ",log->logtim));
	switch(log->logtype)
	{
	case LOG_TYP_TSKSTAT:
		DebugOut(::wsprintfA(Buffer,"[STATUS] Tsk:%d ",*(int*)log->buf));
		switch(*((int*)log->buf+1))
		{
		case 0x01:
		case 0x02:
			DebugOut(::wsprintfA(Buffer,"<Runnable [Ready+Running]>\n",*(int*)log->buf));
			break;
		case 0x04:
		case 0x0c:
			if(*((int*)log->buf+1) == 0x04)
				DebugOut(::wsprintfA(Buffer,"<Waiting : "));
			else
				DebugOut(::wsprintfA(Buffer,"<Wait-Suspend : "));

			switch(*((int*)log->buf+2))
			{
			case 0x1:
				DebugOut(::wsprintfA(Buffer,"Sleep>\n"));
				break;
			case 0x2:
				DebugOut(::wsprintfA(Buffer,"Delay>\n"));
				break;
			case 0x4:
				DebugOut(::wsprintfA(Buffer,"Semaphore>\n"));
				break;
			default:
				DebugOut(::wsprintfA(Buffer," complex [%04x]>\n", *((int*)log->buf+2)));
				break;
			}

			break;
		case 0x08:
			DebugOut(::wsprintfA(Buffer,"<Suspended>\n"));
			break;
		case 0x10:
			DebugOut(::wsprintfA(Buffer,"<Dormant>\n"));
			break;
		default:
			DebugOut(::wsprintfA(Buffer,"<Unknown : %d>",*(int*)log->buf));
			break;
		}
		break;

	case LOG_TYP_TSKEXC:
		DebugOut(::wsprintfA(Buffer,"[TSKEXC|ENT] Tsk:%d\n",*(int*)log->buf));
		break;

	case LOG_TYP_TSKEXC | LOG_LEAVE:
		DebugOut(::wsprintfA(Buffer,"[TSKEXC|LEA] Tsk:%d\n",*(int*)log->buf));
		break;

	case LOG_TYP_DISPATCH:
		if(*((int*)log->buf+1) == 0)
			DebugOut(::wsprintfA(Buffer,"[DSP|ENT] Tsk:%d Task-context\n",*(int*)log->buf));
		else
			DebugOut(::wsprintfA(Buffer,"[DSP|ENT] Tsk:%d Nontask-context\n",*(int*)log->buf));
		break;

	case LOG_TYP_DISPATCH | LOG_LEAVE:
		DebugOut(::wsprintfA(Buffer,"[DSP|LEA] Tsk:%d\n",*(int*)log->buf));		
		break;

	case LOG_TYP_COMMENT:
		DebugOut(::wsprintfA(Buffer,"[COMMENT (%d)] : ",log->bufsz));
		::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), log->buf, log->bufsz-1, &Count, NULL);
		break;
	
	case LOG_TYP_INTERRUPT:
		DebugOut(::wsprintfA(Buffer,"[INT|ENT] %d\n",*(int*)log->buf));
		break;

	case LOG_TYP_INTERRUPT | LOG_LEAVE:
		DebugOut(::wsprintfA(Buffer,"[INT|LEA] %d\n",*(int*)log->buf));
		break;

	case LOG_TYP_CPUEXC:
		DebugOut(::wsprintfA(Buffer,"[CPUEXC|ENT] Tsk:%d\n",*(int*)log->buf));
		break;

	case LOG_TYP_CPUEXC | LOG_LEAVE:
		DebugOut(::wsprintfA(Buffer,"[CPUEXC|LEA] Tsk:%d\n",*(int*)log->buf));
		break;

	case LOG_TYP_TIMERHDR:
	case LOG_TYP_TIMERHDR | LOG_LEAVE:
		if((log->logtype & LOG_LEAVE) != 0)
			DebugOut(::wsprintfA(Buffer,"[INT|LEA]"));
		else
			DebugOut(::wsprintfA(Buffer,"[INT|ENT]"));

		if( *(int*)log->buf == 0x8d )
			DebugOut(::wsprintfA(Buffer," cyclic "));
		else
			DebugOut(::wsprintfA(Buffer," OBJ(%02x) ", *(int*)log->buf));

		DebugOut(::wsprintf(Buffer," ID:%d  EXINF:0x%08x\n",*((int*)log->buf+1), *((int*)log->buf+2)));
		break;

	default:
		DebugOut(::wsprintfA(Buffer,"[UNKNOWN:0x%x]\n",log->logtype));
	};
#endif
	
	if(Kernel != kernel)
	{
		DebugOut(::wsprintfA(Buffer,"Request performed by Illegal Kernel\n"));
		return false;
	}

	::EnterCriticalSection(&cs);
	scope = WatcherList.begin();
	while(scope != WatcherList.end())
	{
		(*scope)->Fire_OnLogEvent(sz, data);
		scope ++;
	}
	::LeaveCriticalSection(&cs);

	return true;
}

