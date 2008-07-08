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
 *  @(#) $Id: cpu_defs.c,v 1.14 2003/12/15 07:19:22 takayuki Exp $
 */

#define _WIN32_WINNT 0x400

#include <s_services.h>
#include <hal_msg.h>

/*
 * m68kっぽい割込みエミュレータ (Windows HAL)
 */

	/* 現在の割込みマスクレベル */
unsigned int CurrentInterruptLevel;

	/* 割込み管理テーブル */
static struct tagInterruptLevel InterruptLevel[INT_NUMINTERRUPTS];

	/* 割込みマスクレベルの有効範囲チェック */
#define CHECK_IMS(x) ( (x) != 0 && (x) <= INT_NUMINTERRUPTS )

	/* 例外管理テーブル */
struct tagExceptionLevel ExceptionLevel[EXC_MAXITEMS];

	/* 自分の前に登録されていた構造化例外ハンドラのアドレス */
static LPTOP_LEVEL_EXCEPTION_FILTER AnotherExceptionFilter;

/*
 *   システム全体のクリティカルセクション生成ルーチン
 *     ・どうしてCRITICAL_SECTIONを使うのをやめたのか
 *        -> Windowsのクリティカルセクションは、ITRONで言うところのディスパッチ禁止で実装しているらしく、
 *           外部イベントで起床してしまったスレッドがクリティカルセクション内にいるスレッドの動作を
 *           停止させてしまうことができるらしい。なのでCPUロックのつもりでクリティカルセクションを使っていると、
 *           実はただのディスパッチ禁止なのでハンドラが起動してしまい、PrimaryThreadがSuspendThreadを発行してデッドロックしてしまう。
 *           この問題は時々発生する非常に厄介な問題だったが、まさかそんなことが原因だとは思いもよらなかった。
 *           WinProg系のMLでも「原因不明の不正同期がイヤならクリティカルセクションは使わないほうがよい」といっている。
 */

static HANDLE SystemMutex = NULL;		                //システムロック用のミューテックスオブジェクト
static DWORD  SystemMutexOwnerThreadID = 0;				//システムロックを取得したスレッドのID
static DWORD  SystemMutexLastOwnerThreadID = 0;			//最後にシステムロックを取得していたスレッドのID (デバッグ用)

	/*
	 *   enter_system_critical_section : システムのロック権を獲得する
	 *     BOOL * lock : クッキー(自分が始めてロック権を獲得するとTRUEが返る)
	 */
void enter_system_critical_section(BOOL * lock)
{
	if(SystemMutex != NULL && SystemMutexOwnerThreadID != GetCurrentThreadId())
	{
		WaitForSingleObject(SystemMutex, INFINITE);	
		if(lock != NULL)
			*lock = TRUE;
		SystemMutexOwnerThreadID = GetCurrentThreadId();
	}else
	{
		if(lock != NULL)
			*lock = FALSE;
	}
}

	/*
	 *   leave_system_critical_section : システムのロック権を開放する
	 *		BOOL * lock : enter_system_critical_sectionで使用したクッキー
	 */
void leave_system_critiacl_section(BOOL * lock)
{
	assert(lock != NULL);

	if(*lock == TRUE)
	{
		SystemMutexLastOwnerThreadID = SystemMutexOwnerThreadID;
		SystemMutexOwnerThreadID = 0;
		if(SystemMutex != NULL)
			ReleaseMutex(SystemMutex);	
	}
}

	/*
	 *   wait_for_thread_suspension_completion : スレッドをロック権開放状態で停止させる 
	 */
ER wait_for_thread_suspension_completion(HANDLE thread)
{
	BOOL lock;

	if(thread == 0 || thread == NULL)
		return E_PAR;

	enter_system_critical_section(&lock);
	SuspendThread(thread);
	leave_system_critiacl_section(&lock);

	return 0;
}



/*
 *  次に実行すべき割込みハンドラのあるレベルを取得する
 *      ipl : 割込みマスクレベル
 *
 *    割り込みレベルマスクがiplまで下がったときに、次に動かすべき割込みを選択する。
 *    マスクレベルよりも上位の割込みの場合、ペンディングされた要求 or 既に実行が
 *    始まっているハンドラ を実行する。マスクレベル以下の場合、既に動いている
 *    ハンドラがある場合のみ実行する。何もない場合は0を返す。
 */
static unsigned int
isns_int( unsigned int ipl )
{
	unsigned int result = INT_NUMINTERRUPTS;
	
	if((CPUStatus & CPU_STAT_LOCK) != 0)
		return 0;

	while(result > ipl)
	{
		result --;
		if( (InterruptLevel[result].Flags & (INT_STAT_PENDING|INT_STAT_RUNNING)) != 0)
			return result+1;
	}
	while(result > 0)
	{
		result --;
		if( (InterruptLevel[result].Flags & INT_STAT_RUNNING) != 0)
			return result+1;
	}
	return 0;
}

static DWORD WINAPI
InterruptHandlerWrapper(LPVOID param)
{
	unsigned int   i;
	unsigned int   PrevLevel;
	BOOL           systemLock;

	struct tagInterruptLevel * intlv = (struct tagInterruptLevel *)param;

	TlsAlloc();

    kprintf(("InterruptHandlerWrapper (%d) : start\n", (intlv - InterruptLevel) + 1 ));

	while(1==1)
	{
			/* 割込み前処理 */

		set_logcontrol((intlv->Flags & INT_MOD_LOGMASK) != 0);
		LOG_INH_ENTER((intlv - InterruptLevel)+1);

        assert((CPUStatus & CPU_STAT_LOCK) == 0);
        enter_system_critical_section(&systemLock);
        PrevLevel = CurrentInterruptLevel;
		CurrentInterruptLevel = (unsigned int)(intlv - InterruptLevel)+1;
		intlv->Flags &= ~INT_STAT_PENDING;
		intlv->Flags |=  INT_STAT_RUNNING;
		leave_system_critiacl_section(&systemLock);

        ( (void (*)(void) )(intlv->Routine))();
        
			/* 割込み後処理 
             *    ここで一気にPrevLevelまで落としてはいけない.
             *    割り込みの入り方によっては、優先度の低い割込みの脱出処理が優先度の高い割り込みの進入処理の直後に実行され、
             *    運が悪いと優先度の高い割込みがCurrentInterruptLevel = 0の状態で走ってしまうことになる。
             */
		enter_system_critical_section(&systemLock);
		intlv->Flags &= ~INT_STAT_RUNNING;
		i = isns_int(PrevLevel);
		if(i > PrevLevel)
			CurrentInterruptLevel = i - 1;
		else
			CurrentInterruptLevel = PrevLevel;
		leave_system_critiacl_section(&systemLock);

		LOG_INH_LEAVE((intlv - InterruptLevel)+1);

            /* 次の割込みを処理するように通知 */
	    HALInterruptRequestAndWait();
	}

    kprintf(("InterruptHandlerWrapper (%d) : exit\n", (intlv - InterruptLevel) + 1 ));

    ExitThread(0);
	return 0;
}

BOOL
def_int(unsigned int ims, void * rtn)
{
	BOOL lock;

	if(!CHECK_IMS(ims) || rtn == NULL)
		return FALSE;

    kprintf(("def_int : [ims:%d]\n", ims));

	ims--;

	enter_system_critical_section(&lock);
	if(InterruptLevel[ims].ThreadHandle != NULL)
	{
		TerminateThread(InterruptLevel[ims].ThreadHandle,0);
		CloseHandle(InterruptLevel[ims].ThreadHandle);
	}
	InterruptLevel[ims].Routine = rtn;
	InterruptLevel[ims].ThreadHandle = CreateThread(NULL,0,InterruptHandlerWrapper,(LPVOID)&InterruptLevel[ims],CREATE_SUSPENDED,&InterruptLevel[ims].ThreadID);
	leave_system_critiacl_section(&lock);

	return TRUE;
}

BOOL
ini_int(void)
{
	int i;

    kprintf(("ini_int : [Start]\n"));

    SystemMutex = CreateMutex(NULL,TRUE,NULL);

	for(i=0;i<INT_NUMINTERRUPTS;i++)
	{
		InterruptLevel[i].Routine      = (void *)0l;
		InterruptLevel[i].ThreadHandle = NULL;
		InterruptLevel[i].ThreadID     = 0;
		InterruptLevel[i].Flags        = INT_MOD_LOGMASK;
	}

		/* 初期起動でCPUロック状態へ移行させる */
	CurrentInterruptLevel = 0;	//割込みマスクは全許可
	CPUStatus |= CPU_STAT_LOCK;	//CPUで割込み禁止に

	ReleaseMutex(SystemMutex);

    kprintf(("ini_int : [Exit]\n"));

	return TRUE;
}

void
fin_int(void)
{
	int    i;
	HANDLE work;

    kprintf(("fin_int : [Start]\n"));

	enter_system_critical_section(NULL);

	for(i=0;i<INT_NUMINTERRUPTS;i++)
	{
		if(InterruptLevel[i].ThreadHandle != NULL)
		{
			TerminateThread(InterruptLevel[i].ThreadHandle,0);
			CloseHandle(InterruptLevel[i].ThreadHandle);
		}
		InterruptLevel[i].Routine = (void *)0l;
		InterruptLevel[i].ThreadHandle = NULL;
		InterruptLevel[i].ThreadID = 0;
		InterruptLevel[i].Flags    = 0;
	}
	CurrentInterruptLevel = 0;

        /* CloseHandle(SystemMutex), SystemMutex = INVALID_HANDLE; を安全にやる */
	work = SystemMutex;
	SystemMutex = NULL;
	CloseHandle(work);

		//これ以降はCPUロック状態だと思い込む
	CPUStatus |= CPU_STAT_LOCK;

    kprintf(("fin_int : [Exit]\n"));
}

BOOL
ras_int(unsigned int ims)
{
	BOOL lock;
	BOOL result = TRUE;

	if(!CHECK_IMS(ims))
		return FALSE;

	ims --;

	enter_system_critical_section(&lock);
	if(InterruptLevel[ims].ThreadHandle == NULL)
	{
		result = FALSE;
	}else
		InterruptLevel[ims].Flags |= INT_STAT_PENDING;
	leave_system_critiacl_section(&lock);
	return result;
}

unsigned int
sns_int( void )
{
	BOOL lock;
	int  result;

	if(sense_lock() == TRUE)
		return 0;

	enter_system_critical_section(&lock);
	result = isns_int(CurrentInterruptLevel);
	leave_system_critiacl_section(&lock);
	return result;
}

HANDLE
sch_int( void )
{
	BOOL   lock;
	HANDLE result;
	unsigned int level;

	if(sense_lock() == TRUE)
		return NULL;

	enter_system_critical_section(&lock);
	level = isns_int(CurrentInterruptLevel);
	if(level != 0)
	{
		result = InterruptLevel[level-1].ThreadHandle;
	}else
		result = NULL;
	leave_system_critiacl_section(&lock);

	return result;
}

DWORD LockerThreadID = 0;

ER
ena_int(unsigned int ims)
{
	BOOL lock;
	int  i;

	enter_system_critical_section(&lock);
	i = isns_int(CurrentInterruptLevel);
	CPUStatus &= ~CPU_STAT_LOCK;
	LockerThreadID = 0;
	leave_system_critiacl_section(&lock);

	if(i != 0)
		HALInterruptRequest(0);

	return 0 /*E_OK*/;
}

ER
dis_int(unsigned int ims)
{
	BOOL lock;

	enter_system_critical_section(&lock);
	CPUStatus |= CPU_STAT_LOCK;
	if(LockerThreadID == 0)
	{
		LockerThreadID = GetCurrentThreadId();
	}else
	{
		DWORD newThreadID = GetCurrentThreadId();
		LockerThreadID = GetCurrentThreadId();	//ブレーク置き場
	}
	leave_system_critiacl_section(&lock);

	return 0;
}

ER
chg_ims(unsigned int ims)
{
	BOOL lock;
	int  i;

		//chg_ims は ims==0 を許す
	if(ims != 0 && !CHECK_IMS(ims))
		return -17 /*E_PAR*/;

	enter_system_critical_section(&lock);
	CurrentInterruptLevel = ims;
	if(CurrentInterruptLevel == INT_NUMINTERRUPTS)
		CPUStatus |= CPU_STAT_LOCK;
	else
		CPUStatus &= ~CPU_STAT_LOCK;
	i = isns_int(ims);
	leave_system_critiacl_section(&lock);

	if(i != 0)
		HALInterruptRequest(0);

	return 0 /*E_OK*/;
}

ER
get_ims(unsigned int *p_ims)
{
	BOOL lock;

	if(p_ims == (void *)0l)
		return -17 /*E_PAR*/;

	enter_system_critical_section(&lock);
	if((CPUStatus & CPU_STAT_LOCK) != 0)
		*p_ims = INT_NUMINTERRUPTS;
	else
		*p_ims = CurrentInterruptLevel;
	leave_system_critiacl_section(&lock);
	return 0 /*E_OK*/;
}

ER
vget_ims(unsigned int *p_ims)
{
	BOOL lock;

	if(p_ims == (void *)0l)
		return -17 /*E_PAR*/;

	enter_system_critical_section(&lock);
	*p_ims = CurrentInterruptLevel;
	leave_system_critiacl_section(&lock);
	return 0 /*E_OK*/;
}

/*
 * 最上位レベルWindows構造化例外ハンドラ
 */

LONG WINAPI
HALExceptionHandler( EXCEPTION_POINTERS * exc )
{
	int i;
	int result;

	if((CPUStatus & CPU_STAT_EXC) == 0)
		CPUStatus |= CPU_STAT_DOUBLEFAULT;

        /* 馬鹿サーチ (空間がせまいから...) */
	CPUStatus |= CPU_STAT_EXC;
	for(i=0;i<EXC_MAXITEMS;i++)
	{
		if(ExceptionLevel[i].ExceptionCode == exc->ExceptionRecord->ExceptionCode)
		{
			result = EXCEPTION_CONTINUE_SEARCH;
	
			LOG_EXC_ENTER(i);
			( * ((void (*)(void *,int *))ExceptionLevel[i].Routine)) (exc,&i);
			LOG_EXC_LEAVE(i);

			CPUStatus &= ~CPU_STAT_EXC;
			return result;
		}
	}
	CPUStatus &= ~CPU_STAT_EXC;
	return EXCEPTION_CONTINUE_SEARCH;
}

BOOL
ini_exc(void)
{
	int i;

	for(i=0;i<EXC_MAXITEMS;i++)
	{
		ExceptionLevel[i].ExceptionCode = 0;
		ExceptionLevel[i].Routine = 0l;
	}

	AnotherExceptionFilter = SetUnhandledExceptionFilter(HALExceptionHandler);
	
	return TRUE;
}

void
fin_exc(void)
{
    SetUnhandledExceptionFilter(AnotherExceptionFilter);
}

BOOL
def_exc(DWORD exc, void * routine)
{
	int j;
	int i;

        /* 解除ルーチン */
	if(routine == 0l)
	{
        kprintf(("def_exc : [UNREG] 0x%08x\n", exc));

		for(i=0;i<EXC_MAXITEMS;i++)
			if(ExceptionLevel[i].ExceptionCode == exc)
			{
				ExceptionLevel[i].ExceptionCode = 0;
				ExceptionLevel[i].Routine = 0;
				return TRUE;
			}
		return FALSE;
	}

    /* 登録ルーチン */

    kprintf(("def_exc : [REG] 0x%08x\n", exc));

	j = EXC_MAXITEMS;
	for(i=0;i<EXC_MAXITEMS;i++)
	{
		if(ExceptionLevel[i].ExceptionCode != 0)
		{
				/* 登録しようとしている番号が既に登録されていないかどうか調べる */
			if(ExceptionLevel[i].ExceptionCode == exc)
				return FALSE;
		}else
		{
				/* 空き番号の最小を覚える */
			if(j > i)
				j = i;

            /* 既登録チェックがあるので、ここでbreakしてはダメ */
		}
	}

	FatalAssertion(i == EXC_MAXITEMS, "No available exception slots left.");

		//This sequence will never change
	ExceptionLevel[j].Routine       = routine;
	ExceptionLevel[j].ExceptionCode = exc;

	return TRUE;
}

ER set_intlogmask( unsigned int ims, BOOL mask )
{
	BOOL lock;

	if( !CHECK_IMS(ims) )
		return FALSE;

	-- ims;

	enter_system_critical_section(&lock);
	if(mask == TRUE)
		InterruptLevel[ims].Flags |= INT_MOD_LOGMASK;
	else
		InterruptLevel[ims].Flags &= ~INT_MOD_LOGMASK;
	leave_system_critiacl_section(&lock);

	return TRUE;
}

unsigned int current_timer_clock_unit = TIMER_CLOCK_WINDOWS;

ER set_clk(unsigned int clk)
{
    BOOL lock;

    if(clk == 0)
        clk = TIMER_CLOCK_WINDOWS;

    hw_timer_terminate();
    enter_system_critical_section(&lock);
    current_timer_clock_unit = clk;
    leave_system_critiacl_section(&lock);
    hw_timer_initialize();

    return E_OK;
}
