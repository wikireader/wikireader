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
 *  @(#) $Id: cpu_config.c,v 1.15 2003/12/15 07:19:22 takayuki Exp $
 */


/*
 *	プロセッサ依存モジュール（Windows用）
 */
#include <cpu_rename.h>

#include <s_services.h>

#include "jsp_kernel.h"
#include "check.h"
#include "task.h"

#include <hal_msg.h>

volatile char CPUStatus = CPU_STAT_LOCK;	//CPU状態フラグ

/*
 *  タスクディスパッチャ
 *    ディスパッチャ本体はprimary_thread.cにある
 *    ここでは管理スレッドにディスパッチ依頼を出すだけ
 */
void
dispatch()
{
	LOG_DSP_ENTER_TSK(runtsk);
	HALDispatchRequest();	
}

/* 割込み用ディスパッチャ (出力するログ情報のみが異なる) */
void
idispatch()
{
	LOG_DSP_ENTER_INT(runtsk);
	HALDispatchRequest();
}


/*
 *  現在のコンテキストを捨ててディスパッチ
 *    exit_and_dispatch は，CPUロック状態で呼び出さなければならない．
 *
 *    現在実行中のタスクのTCBを指定して破棄依頼。破棄後自動的にディスパッチャへ。
 */
void
exit_and_dispatch() 
{
    HANDLE handle;

	HALDestroyRequest(runtsk);

    if((handle = (HANDLE)TlsGetValue(TLS_THREADHANDLE)) != NULL)
        CloseHandle(handle);

    TlsFree(TLS_THREADHANDLE);
    TlsFree(TLS_LOGMASK);

	ExitThread(0);
}


/*
 *  プロセッサ依存の初期化
 */

void
cpu_initialize()
{
    kprintf(("cpu_initialize()\n"));

	/*
	 *  割込みエミュレータの初期化
	 */
	ini_int();

	/*
	 * 最上位構造化例外ハンドラの設定
	 */
	ini_exc();
}

/*
 *  プロセッサ依存の終了処理
 */
void
cpu_terminate()
{
    kprintf(("cpu_terminate()\n"));

    fin_int();  //割込みエミュレータの停止
	fin_exc();  //最上位構造化例外ハンドラの解除
}


/*
 * タスク起動ルーチン
 *
 */

DWORD WINAPI
activate_r( LPVOID param)
{
	TCB * tcb = (TCB *)param;

	TlsAlloc();
	TlsSetValue(TLS_LOGMASK, 0);
    TlsSetValue(TLS_THREADHANDLE, tcb->tskctxb.ThreadHandle);

    kprintf(("Task %d : activated\n", (tcb - tcb_table) + 1));

	(*(void(*)(VP_INT))tcb->tinib->task)(tcb->tinib->exinf);
	ext_tsk();
	return 0;
}

/*
 *  atexitで登録された関数の読出し (Windowsは何もしない -> CRTに任せる)
 */
void call_atexit(void)
{}


/*
 *   致命的な失敗に対する対処ルーチン
 */
void FatalAssertion(int exp, LPCSTR format, ...)
{
	extern HANDLE PrimaryThreadHandle;
	extern HANDLE CurrentRunningThreadHandle;

	if(!exp)
	{
		va_list vl;
		char    buffer[1024];

			/* エラーが起きた原因を突き止める */
		wsprintf(buffer, "Critical assertion fail occured !!\nGetLastError = 0x%08x\n\n", GetLastError());

			/* カーネルが動き出していたら... */		
		if(PrimaryThreadHandle != NULL)
		{
				/* カーネルの実行を止める */
			dis_int(0);
			if(CurrentRunningThreadHandle != NULL)
				SuspendThread(CurrentRunningThreadHandle);
			hw_timer_terminate();
		}

			/* 警告表示 */
		va_start(vl, format);
		wvsprintf(buffer + lstrlen(buffer), format, vl);
		MessageBox(PrimaryDialogHandle, buffer, "Assertion Failed", MB_OK|MB_ICONERROR);

			/* カーネル停止 */
		HALQuitRequest();
	}
}

	/*
	 *   ログ出力をどうするかの制御
	 */
void set_logcontrol(BOOL enable)
{
	BOOL result;
	result = TlsSetValue(TLS_LOGMASK, (LPVOID)(enable == TRUE ? 0 : 1));
	if(result == 0)
	{
		TlsAlloc();
		TlsSetValue(TLS_LOGMASK, (LPVOID)(enable == TRUE ? 0 : 1));
	}
}

    /*
     *   カーネルのデバッグ用printf
     */
void _kernel_debugprintf(const char * format, ... )
{
    va_list vl;
    int     words;
    DWORD   count;
    char    buffer[1024];

    va_start(vl, format);
    words = wvsprintf(buffer, format, vl);

    if(words != 0)
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, words, &count, NULL);
}

#ifdef _MSC_VER     //cygwinの場合はcpu_insn.Sに記述

 /*
  * タスク例外起動ルーチン
  *   コンテキストを一致させるため、
  *   タスク例外を動かしたタスクのスレッド上で動作させるためのルーチン
  */
void __declspec(naked) TaskExceptionPerformer(void)
{
		/* レジスタ退避 */
	__asm	pusha
	__asm	sub		esp, FPU_CONTEXTSIZE
	__asm	fsave	[esp]

		/* タスク例外の起動 */
	__asm	call	call_texrtn

		/* 割込みマスク戻し */
	__asm	mov		ebx, DWORD PTR runtsk
	__asm	mov		eax, [ebx]TCB.tskctxb.InterruptLevel
	__asm	push	eax
	__asm	call	chg_ims
	__asm	add		esp, 4

		/* レジスタ復帰 */
	__asm	finit
	__asm	frstor	[esp]
	__asm	add		esp, FPU_CONTEXTSIZE
	__asm	popa
	__asm	ret
}

#endif

