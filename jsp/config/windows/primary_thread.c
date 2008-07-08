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
 *  @(#) $Id: primary_thread.c,v 1.10 2007/04/19 07:44:46 honda Exp $
 */

#include <vwindows.h>
#include <hal_msg.h>
#include <hw_timer.h>

#include <objbase.h>
#include <shellapi.h>
#include <resource.h>

#include "jsp_kernel.h"
#include "task.h"
#include <eventlog.h>
#include <cpu_rename.h>

    /* 終了時に破棄を行う関数のキュー */
struct tagDestructionProcedureQueue
{
	struct tagDestructionProcedureQueue * Next;
	void (*DestructionProcedure)(void *);
	void * Parameter;
};

 /*
  *   プロトタイプ宣言
  */
extern void kernel_start();
extern void kernel_exit();

 /*
  *   大域変数
  */
HINSTANCE ProcessInstance;
HANDLE    PrimaryThreadHandle;
HWND      PrimaryDialogHandle;
HANDLE    CurrentRunningThreadHandle;
BOOL      ShutdownPostponementRequest;

static HANDLE WorkerThreadHandle = NULL;
static struct tagDestructionProcedureQueue * DestructionProcedureQueue;


 /*
  *  現在実行中のスレッドが指定されたタスクであるかどうかのチェック
  */
Inline 
int isTaskThreadRunning(TCB * tcb)
{   return (tcb != 0) && (tcb->tskctxb.ThreadHandle == CurrentRunningThreadHandle);   }


 /*
  * カーネルスタータ
  *   kernel_startは最後にexit_and_dispatchを呼ぶので、コンテキスト破棄に
  *   備えて新しいスレッドを生成する。
  */

static DWORD WINAPI
KernelStarter(LPVOID param)
{
    TlsAlloc();
	TlsSetValue(TLS_LOGMASK, (LPVOID)1);
	TlsSetValue(TLS_THREADHANDLE, (LPVOID)CurrentRunningThreadHandle);

    kprintf(("KernelStarter begins performing kernel initialization.\n"));

    kernel_start();

    /* プログラムはここには来ない */

	return 0;
}

 /*
  *  タスク例外起動ルーチン 
  *     Visual C++   -> cpu_config.c に移動しました
  *     mingw/cygwin -> cpu_insn.S   に移動しました
  */
extern void TaskExceptionPerformer(void);

 /*
  * 強制遮断ルーチン
  *   応答を返さないオブジェクト破棄プロシジャ対策
  */
DWORD WINAPI
ForceShutdownHandler(LPVOID param)
{
	do {
		ShutdownPostponementRequest = FALSE;
		Sleep(5000);
	} while(ShutdownPostponementRequest == TRUE);
	ExitProcess(0);
	return 0;
}

 /*
  * デバッグ時用ダイアログのメッセージハンドラ
  */
Inline LRESULT CALLBACK
PrimaryDialogCommandHandler(WPARAM wParam, LPARAM lParam)
{
	static BOOL lock_flag;

	switch(wParam)
	{
			/* 「クロックの供給を停止する」ボタン */
		case IDC_CLOCKSUPPLY:
		{
			int state;

			state = SendDlgItemMessage(PrimaryDialogHandle, IDC_CLOCKSUPPLY,BM_GETCHECK,0,0);
			switch(state)
			{
					/* クロック停止 -> クロックを止める and 今動いているスレッドを強制停止 */
			case BST_CHECKED:
				if((lock_flag = sense_lock()) != TRUE)
					dis_int(0);
				hw_timer_terminate();
				if(CurrentRunningThreadHandle != NULL)
					SuspendThread(CurrentRunningThreadHandle);
				break;

					/* クロック供給再開 -> 最後に止めたスレッドの再開 and クロック供給再開 */
			case BST_UNCHECKED:
				if(CurrentRunningThreadHandle != NULL)
					ResumeThread(CurrentRunningThreadHandle);
				hw_timer_initialize();
				if(lock_flag != TRUE)
					ena_int(0);
				break;
			}
			break;
		}

	default:
		return FALSE;
	}

	return TRUE;
}

    /*
     *  TOPPERS/JSP スレッドモデル タスクディスパッチャ 
     */
static void task_dispatcher(int is_taskschedule_required)
{
		/* いま動いているスレッド(=割込み+タスク)があれば、それを止める */
	if(CurrentRunningThreadHandle != NULL)
	{
		wait_for_thread_suspension_completion(CurrentRunningThreadHandle);

            //動いていたのがタスクであれば、割込みマスクレベルを保存する
        if(isTaskThreadRunning(runtsk))
			vget_ims(&runtsk->tskctxb.InterruptLevel);
	}

		/* タスク切替 */

        //ディスパッチする必要がある
    if(is_taskschedule_required != 0 && enadsp && runtsk != schedtsk)
        runtsk = schedtsk;

        //切換先タスクが存在するなら、そのタスクを起動する
	if(runtsk != 0l)
	{
		CurrentRunningThreadHandle = runtsk->tskctxb.ThreadHandle;

			/* タスク例外がおこったら */
		if (runtsk->enatex && runtsk->texptn != 0) 
		{
				/* タスク例外起動ルーチンへと差し替える */
			CONTEXT context;
			context.ContextFlags = CONTEXT_FULL;
			GetThreadContext(CurrentRunningThreadHandle,&context);
			*(DWORD *)(context.Esp -= 4) = context.Eip;
			context.Eip = (DWORD)TaskExceptionPerformer;
			SetThreadContext(CurrentRunningThreadHandle,&context);
		}else
			chg_ims(runtsk->tskctxb.InterruptLevel);
		
		LOG_DSP_LEAVE(runtsk);
		ResumeThread(runtsk->tskctxb.ThreadHandle);
	}else
	{
			/* 動かすものがないなら、割り込みをあけて待つ */
		CurrentRunningThreadHandle = NULL;
		ena_int(0);
	}
}



	/*
	 * TOPPERS/JSP スレッドモデル カーネルメッセージハンドラ
	 */
Inline LRESULT CALLBACK
HALMessageHandler(WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
			/*
			 *「タスクを破棄してください」メッセージ 
			 *  lParam : 破棄対象タスクのTCBのアドレス
			 */
	case HALMSG_DESTROY:
        {
            TCB * tcb = (TCB *)lParam;

				    /* tcb == 0 が成立するのは、KernelStarterがexit_and_dispatchしたときのみ */
            if(tcb == 0 || isTaskThreadRunning(tcb)){
                CurrentRunningThreadHandle = NULL;

                    /* タスクが次の起動要求を出してext_tskすると、この時点ですでに新しいスレッドのハンドルが入っているので消してはいけない */

                if(tcb == runtsk)
                    runtsk = 0;
            }
        }

			/*
			 *「タスクを切り替えてください」メッセージ
			 */
	case HALMSG_DISPATCH:
        task_dispatcher(1);
        break;

		/*
		 *「割り込みを発生させてください」メッセージ
		 * lParam : 割込み番号 (>0)
		 */
	case HALMSG_INTERRUPT:
        if(lParam == 0 || iniflg == FALSE || ras_int((unsigned int)lParam) == FALSE)
            break;

            /* 割込み受付処理 : このまま次に */

        /*
         * 「次に実行すべき割り込みを探して、割込み処理を開始してください」メッセージ
         */
    case HALMSG_INTERRUPT_FINISH:
        {
			    /* 現在実行しているスレッドを停止 */
		    wait_for_thread_suspension_completion(CurrentRunningThreadHandle);

				/* 割込みスレッド生成 and ディスパッチ */
			if((CurrentRunningThreadHandle = sch_int()) != NULL)
			{
                    //これまで動かしていたタスクの割込みマスクレベルを退避
				if(isTaskThreadRunning(runtsk))
					vget_ims(&runtsk->tskctxb.InterruptLevel);

                ResumeThread(CurrentRunningThreadHandle);   //割込みスレッド起動
			}
            else {
                    // タスクへと戻る
                task_dispatcher(reqflg);
                reqflg = 0;
            }

			break;
		}

		/*
		 *「管理スレッドの権限でもって関数を実行してください」メッセージ
		 *  lParam : パラメータを格納する構造体へのポインタ
		 *    パラメータ構造体内訳
		 *      func  : 実行したい関数
		 *      param : パラメータとして渡す何でもアリ("void *")な値
		 *
		 * 注) タスクでウィンドウとかを作ると、タスク破棄でウィンドウが消えてしまうよ
		 */
	case HALMSG_EXECUTEPROCEDURE:
		{
			void ** work = (void **)lParam;
			((void (*)(void *))(*work))(*(work+1));
			break;
		}

		/*
		 *「最後の最後にこの処理を動かしてください(onExitハンドラ)」メッセージ
		 * lParam : 関数実行メッセージといっしょ ( func,paramへのポインタ )
		 */
	case HALMSG_ADDDESTRUCTIONPROCEDURE:
		{
			struct tagDestructionProcedureQueue * scope;
			void ** work = (void **)lParam;

			scope = DestructionProcedureQueue;

			if((DestructionProcedureQueue = GlobalAlloc(GMEM_FIXED, sizeof(struct tagDestructionProcedureQueue))) != NULL)
			{
				DestructionProcedureQueue->DestructionProcedure = *(work);
				DestructionProcedureQueue->Parameter = *(work+1);
				DestructionProcedureQueue->Next = scope;
			}else
				FatalAssertion(TRUE, "GlobalAlloc could not acquire a memory block at " __FILE__);

			break;
		}

		/*
		 *「プログラムを止めてください」メッセージ
		 */
	case HALMSG_QUITREQUEST:
		{
			struct tagDestructionProcedureQueue * destqueue;
			void * destarea;

			dis_int(0);		// 割込み受付もこのスレッドがやるので、別に禁止しなくても大丈夫

//			WorkerThreadHandle = CreateThread(NULL, 0, ForceShutdownHandler, 0, NULL, NULL);

			hw_timer_terminate();

			if(CurrentRunningThreadHandle != NULL)
				wait_for_thread_suspension_completion(CurrentRunningThreadHandle);

			destqueue = DestructionProcedureQueue;
			while(destqueue != NULL)
			{
				(*destqueue->DestructionProcedure)(destqueue->Parameter);
				destarea = destqueue;
				destqueue = destqueue->Next;
				GlobalFree((HGLOBAL)destarea);
			}

			DestroyWindow(PrimaryDialogHandle);
			break;
		}

	default:
		return FALSE;
	}
	return TRUE;
}

/*
 * カーネルシミュレータの中核となるスレッドのメッセージハンドラ
 */
LRESULT CALLBACK PrimaryDialogProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
		/* タスクトレイアイコンで右クリック */
	case HALMSG_MESSAGE+1:
		if(lParam == WM_RBUTTONUP)
			kernel_exit();

		break;

		/* カーネルメッセージ */
	case HALMSG_MESSAGE:
		return HALMessageHandler(wParam,lParam);

		/* デバッグ用ダイアログ上のアイテムのメッセージ */
	case WM_COMMAND:
		return PrimaryDialogCommandHandler(wParam,lParam);
	
		/* ダイアログ生成(メッセージハンドリング用) */
	case WM_INITDIALOG:
		{
			DWORD ThreadID;
			NOTIFYICONDATA nid;

			PrimaryDialogHandle = hDlg;	//一時的

				/* タスクトレイにアイコンを登録 */
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE;
			nid.uID = ID_NOTIFYICON;
			lstrcpy(nid.szTip,"TOPPERS/JSP");
			nid.hWnd = hDlg;
			nid.uCallbackMessage = HALMSG_MESSAGE+1;
			nid.hIcon = LoadIcon(ProcessInstance,MAKEINTRESOURCE(IDI_ERTLICON));

			Shell_NotifyIcon(NIM_ADD,&nid);

				/* カーネルを別スレッドで起動する */
			PrimaryThreadHandle = (HANDLE)hDlg;
			CurrentRunningThreadHandle = CreateThread(NULL,0,KernelStarter,NULL,CREATE_SUSPENDED,&ThreadID);
            ResumeThread(CurrentRunningThreadHandle);

			FatalAssertion(CurrentRunningThreadHandle != NULL, "CreateThread at " __FILE__);

			break;
		}

		/* ダイアログを閉じようとしています */
	case WM_CLOSE:
			HALQuitRequest();
			break;

		/* ダイアログが破棄されました */
	case WM_DESTROY:
		{
				/* タスクトレイ始末 */
			NOTIFYICONDATA nid;

			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.uFlags = 0;
			nid.hWnd = hDlg;
			nid.uID = ID_NOTIFYICON;
			Shell_NotifyIcon(NIM_DELETE,&nid);

			PrimaryThreadHandle = NULL;
			PostQuitMessage(0);
			break;
		}

		/* タイマ処理 */
	case WM_TIMER:
			/* 上位16ビットが全部1 -> カーネルのタイマ */
		if((wParam & 0xffff0000) == 0xffff0000)
		{
				/* 下位16ビットは割込み番号 */
			return HALMessageHandler(HALMSG_INTERRUPT,(wParam & 0x0000ffff));
		}
	default:
		return FALSE;
	}
	return TRUE;
}


    /* マルチプロセッサでも安定するよう、わざと単一のプロセッサのみで処理させるようにする */
void setAffinityMask(void)
{
    DWORD process;
    DWORD system;
    DWORD newaffinitymask;

    if(GetProcessAffinityMask(GetCurrentProcess(), &process, &system) != 0) {
        newaffinitymask = 1;
        while((process & newaffinitymask) == 0)
            newaffinitymask <<= 1;
        SetProcessAffinityMask(GetCurrentProcess(), newaffinitymask);
    }

    kprintf(("setAffinityMask : 0x%08x\n", newaffinitymask));
}

static void initialize(HANDLE hInstance)
{
	ProcessInstance            = hInstance;
	DestructionProcedureQueue  = NULL;
	PrimaryThreadHandle        = NULL;
	CurrentRunningThreadHandle = NULL;

#ifdef KERNEL_DEBUG_MODE
    AllocConsole();
#endif
        /* プロセッサを割り付ける */
    setAffinityMask();

}

static void finalRelease(void)
{
	int i;

    kprintf(("finalRelease()\n"));

	/* 破棄されてないタスクの後始末 */
	for(i=0;i<_kernel_tmax_tskid;i++)
	{
		if(_kernel_tcb_table[i].tskctxb.ThreadHandle != NULL)
		{
			if(TerminateThread(_kernel_tcb_table[i].tskctxb.ThreadHandle,0) != 0)
    			CloseHandle(_kernel_tcb_table[i].tskctxb.ThreadHandle);
			_kernel_tcb_table[i].tskctxb.ThreadHandle = NULL;
		}
	}

    /* COM通信をしているスレッドを強制停止 */
	if(WorkerThreadHandle  != NULL)
	{
		TerminateThread(WorkerThreadHandle ,0);
		CloseHandle(WorkerThreadHandle);
        WorkerThreadHandle = NULL;
	}

#ifdef KERNEL_DEBUG_MODE
    MessageBox(NULL, "The kernel will be shut down.", "TOPPERS/JSP", MB_OK);
    FreeConsole();
#endif

}


/*
 * メイン関数
 */
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShow)
{
	MSG msg;
	HANDLE hDlg;

    TlsAlloc();

    initialize(hInstance);

	TlsSetValue(TLS_LOGMASK, 0);
    hDlg = CreateDialog(hInstance,"PrimaryDialog",NULL,PrimaryDialogProc);
	if(hDlg != NULL)
	{		
		ShowWindow(PrimaryDialogHandle,SW_HIDE);

		OnDebug(ShowWindow(PrimaryDialogHandle,SW_SHOW));

		while(GetMessage(&msg,NULL,0,0) != 0)
		{
			if(msg.message == WM_QUIT)
				msg.message = 0;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else
		FatalAssertion(hDlg != NULL, "CreateDialog at " __FILE__ "("  ")");

    finalRelease();

    ExitProcess(msg.wParam);
	return msg.wParam;
}

