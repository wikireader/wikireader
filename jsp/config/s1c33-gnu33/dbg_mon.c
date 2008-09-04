/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2004 by SEIKO EPSON Corp, JAPAN
 * 
 *  上記著作権者は，以下の (1)～(4) の条件か，Free Software Foundation 
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
 */

#include "stdio.h"
#include "string.h"

#include "jsp_kernel.h"
#include "sil.h"

#include "task.h"
#include "cyclic.h"
#include "mailbox.h"
#include "semaphore.h"
#include "wait.h"
#include "eventflag.h"
#include "dataqueue.h"
#include "mempfix.h"
#include "interrupt.h"
#include "exception.h"

#include "dbg_mon.h"
#include "kernel_objs.h"

/*
 *  テーブル生成
 *  Generates tables
 */

const	TPS_OSIDENT_INFO	_kernel_OSIdent =
	{"TOPPERS/JSP\0\0\0\0\0\0\0\0",
	 "1.4.2\0\0\0\0"};

const	TPS_OBJINFO_TABLE	_kernel_tps_ObjInfoTable[22] = {
	/* 基本サイズ			生成数		配置アドレス	*/
	/* Unit size			Number		Address		*/
	{sizeof(tps_IntNestCnt),	1,		(void *)&tps_IntNestCnt},
	{sizeof(enadsp),		1,		(void *)&enadsp},
	{sizeof(runtsk),		1,		(void *)&runtsk},
	{sizeof(schedtsk),		1,		(void *)&schedtsk},
	{sizeof(current_time),		1,		(void *)&current_time},
	{sizeof(ready_queue[0]),	TNUM_TPRI,	(void *)ready_queue},
	{sizeof(TCB),			TNUM_TSKID,	(void *)tcb_table},
	{sizeof(TINIB),			TNUM_TSKID,	(void *)tinib_table},
	{sizeof(CYCCB),			TNUM_CYCID,	(void *)cyccb_table},
	{sizeof(CYCINIB),		TNUM_CYCID,	(void *)cycinib_table},
	{sizeof(INHINIB),		TNUM_INHNO,	(void *)inhinib_table},
	{sizeof(EXCINIB),		TNUM_EXCNO,	(void *)excinib_table},
	{sizeof(MBXCB),			TNUM_MBXID,	(void *)mbxcb_table},
	{sizeof(MBXINIB),		TNUM_MBXID,	(void *)mbxinib_table},
	{sizeof(SEMCB),			TNUM_SEMID,	(void *)semcb_table},
	{sizeof(SEMINIB),		TNUM_SEMID,	(void *)seminib_table},
	{sizeof(FLGCB),			TNUM_FLGID,	(void *)flgcb_table},
	{sizeof(FLGINIB),		TNUM_FLGID,	(void *)flginib_table},
	{sizeof(DTQCB),			TNUM_DTQID,	(void *)dtqcb_table},
	{sizeof(DTQINIB),		TNUM_DTQID,	(void *)dtqinib_table},
	{sizeof(MPFCB),			TNUM_MPFID,	(void *)mpfcb_table},
	{sizeof(MPFINIB),		TNUM_MPFID,	(void *)mpfinib_table}
};

const	unsigned short		_kernel_tps_PropertyTable[6] = {
	TMIN_TPRI,						/* 最小優先度			*/
								/* Minimum priority		*/
	TMAX_TPRI,						/* 最大優先度			*/
								/* Maximum priority		*/
	TIC_NUME,						/* タイムティック周期1		*/
								/* Time tick cycle 1		*/
	TIC_DENO,						/* タイムティック周期2		*/
								/* Time tick cycle 2		*/
	1,							/* システム時刻更新周期		*/
								/* The cycle renewing system clock */
	TPS_CPULOCK_LEV						/* CPUロック割り込みレベル	*/
								/* Interruption level as CPU lock  */
};


TPS_DISPATCH_INFO	_kernel_tps_DispatchInfo;		/* ディスパッチログ記録領域	*/
								/* Dispatch log recording area	*/
TPS_KOBJACCESS_INFO	_kernel_tps_KObjAccessInfo;		/* カーネルオブジェクトアクセス	*/
								/* ログ記録領域			*/
								/* Kernel object access log	*/
								/* Recording area		*/
void 			*_kernel_tps_SymStack[15 + 7];		/* 多重割り込み追跡用シンボルスタック */
								/* Symbol stack to trace multi-	*/
								/* ple interruption		*/
void			*_kernel_tps_CycHdrTorch;		/* 周期ハンドラ起動フラグ	*/
								/* Cyclic handler start flag	*/
static	unsigned char	_kernel_tps_PrevCtxType;		/* 前出ディスパッチログ情報	*/
static	unsigned short	_kernel_tps_PrevCtxId;			/* Dispatch log information	*/
								/* recorded previosly		*/


/************************************************************************
 *  tpsInitMonitor
 *	Type	 : void
 *	Ret val  : none
 *	Argument : VP_INT exinf ... extension parameter.
 *	Function : Initialize any variables of Status moniter module.
 ************************************************************************/
void tpsInitMonitor(VP_INT pExInfo)
{
								/* ログ記録領域制御情報初期化	*/
								/* Initializes Log recording	*/
								/* control area			*/
	_kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount = TPS_MAX_DISPLOG;
	if(0x00010000 < _kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount){
		_kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount = 0x00010000;
	}
	_kernel_tps_DispatchInfo.stRecCtrl.usRefPoint    = 0;	/* O.W.許可フラグの初期化は	*/
	_kernel_tps_DispatchInfo.stRecCtrl.usRecPoint    = 0;	/* IDEの責任で行う		*/
	_kernel_tps_DispatchInfo.stRecCtrl.bOWState      = 0;	/* O.W. Enable flag is initia-	*/
								/* lized by IDE.		*/
	_kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount = TPS_MAX_KOBJLOG;
	if(0x00010000 < _kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount){
		_kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount = 0x00010000;
	}
	_kernel_tps_KObjAccessInfo.stRecCtrl.usRefPoint    = 0;	/* O.W.許可フラグの初期化は	*/
	_kernel_tps_KObjAccessInfo.stRecCtrl.usRecPoint    = 0;	/* IDEの責任で行う		*/
	_kernel_tps_KObjAccessInfo.stRecCtrl.bOWState      = 0;	/* O.W. Enable flag is initia-	*/
							       	/* lized by IDE.		*/
	_kernel_tps_CycHdrTorch = NULL;
	_kernel_tps_PrevCtxType = 0x00;
	_kernel_tps_PrevCtxId   = 0x0000;
	return;
}


/************************************************************************
 *  tpsChkRecAreaFull
 *	Type	 : int
 *	Ret val  : TPS_E_OK ... It is possible to record log.
 *	           TPS_E_NG ... It is not possible to record log.
 *	Argument : TPS_LOGREC_CTRL *pstCtrl ... Pointer to control area.
 *	Function : If O.W. is not followed, and all area is recorded, 
 *                 This function notices that it is not possible to record log.
 ************************************************************************/
int
tpsChkRecAreaFull(TPS_LOGREC_CTRL *pstCtrl, int iLogType)
{
	unsigned short	usRecPoint, usRefPoint;
	int		iRetVal;

	if(!(pstCtrl->ulMaxRecCount)){				/* ログの記録を許可していない	*/
		return	TPS_E_NG;				/* Recording is disallowed	*/
	}

	iRetVal = TPS_E_OK;					/* ログの記録が許可されている	*/
	usRecPoint = pstCtrl->usRecPoint;			/* Recorded normally		*/
	usRefPoint = pstCtrl->usRefPoint;
	if(TPS_FLAG_OFF != pstCtrl->bOWState){
		if(TPS_FLAG_OFF == pstCtrl->bOWEnable){		/* 未使用レコードなし		*/
			iRetVal = TPS_E_NG;			/* All records are used		*/
		}
	}

	return iRetVal;
}


/************************************************************************
 *  tpsSearchCtxId
 *	Type	 : unsigned short
 *	Ret val  : ID or Handler number.
 *                 0xffff(-1) means that recording is not done.
 *	Argument : unsigned char *bCtxType ... Context type
 *		   void *pCBoxAddr         ... Control Box or Initalize 
 *		                              infomation address
 *	Function : This function specifies Task/Cyclic Handler ID or
 *		   Interrupt/Exception handler number.
 *		   If "TPS_CTXTYPE_INTHDR" is specified, this function
 *		   searches in Int-handler information table first.
 *		   But If there is not corresponding information, it searches
 *		   in Exc-handler information table next, when there is
 *		   the information, this function renews specified type with
 *		   "TPS_CTXTYPE_EXCHDR". 
 ************************************************************************/
unsigned short
tpsSearchCtxId(unsigned char *pbCtxType, void *pCBoxAddr)
{
	unsigned short	usRecId, usTemp;

	switch (*pbCtxType){
		case TPS_CTXTYPE_INTHDR:
			if(TPS_C_TIMER_HANDLER == pCBoxAddr){
				usRecId = (unsigned short)-1;
				break;
			}

			usTemp = TNUM_INHNO;			/* 変数に格納して比較するのは	*/
			usRecId = 0;				/* オブジェクト未生成の場合に	*/
								/* ワーニングが発生するため	*/
								/* usTemp is used to restrain	*/
			while((usTemp > usRecId) &&		/* Warning.			*/
			      ((FP)pCBoxAddr != _kernel_inhinib_table[usRecId].inthdr)){
				usRecId++;
			}
			if(usTemp > usRecId){
				usRecId++;
				break;
			}
			/* Search Exception handler information table */
			*pbCtxType = TPS_CTXTYPE_EXCHDR;
			usTemp = TNUM_EXCNO;
			usRecId = 0;
			while((usTemp > usRecId) &&
			      (pCBoxAddr != (void *)_kernel_excinib_table[usRecId].exchdr)){
				usRecId++;
			}
			if(usTemp > usRecId){
				usRecId++;
				break;
			}

			usRecId = (unsigned short)-1 ;		/* マクロ"INT_ENTRY" or "EXC_ENTRY"	*/
			break;					/* により付加された出入り口処理から	*/
								/* 呼び出されるため、該当なしという	*/
								/* ケースは実際には存在しない		*/
		case TPS_CTXTYPE_CYCHDR:			/* Usually, this route is not proccessed*/
			usRecId = (unsigned short)((((CYCCB*)pCBoxAddr) - cyccb_table) + TMIN_CYCID);
			break;
		case TPS_CTXTYPE_TASK:
			usRecId = (unsigned short)TSKID((TCB *)pCBoxAddr);
			break;
		default:
			usRecId = 0;				/* Idleルーチン		*/
			break;					/* Idle routine		*/
	}

	return usRecId;
}


/************************************************************************
 *  tpsRecDispatchLog
 *	Type	 : void
 *	Ret val  : none
 *	Argument : unsigned char bCtxType  ... The context type which shifted
 *	                                       to running state.
 *		   void *pCBAddr ... Normally, Address of a control box of the context
 *				     which gets the running right is specified.
 *			If Int/Exc handler is started, Entry point of the
 *			handler(XXX_entry) is specified.
 *	Function : Records Dispath log with specified parameters.
 ************************************************************************/
void
tpsRecDispatchLog(unsigned char bCtxType, void *pCBAddr)
{
	volatile unsigned short	*pusRecPoint;
	unsigned short		usRecId;
	int			iRetVal;

	iRetVal = tpsChkRecAreaFull((TPS_LOGREC_CTRL *)&_kernel_tps_DispatchInfo,
				    TPS_LOGTYPE_DISP);
	if(TPS_E_NG == iRetVal){				/* オーバーライト禁止状態での	*/
		return;						/* 空きレコードなし		*/
	}							/* Log recording is impossible	*/

	pusRecPoint = &(_kernel_tps_DispatchInfo.stRecCtrl.usRecPoint);
	usRecId = tpsSearchCtxId(&bCtxType, pCBAddr);		/* タスク/周期ハンドラID or	*/
								/* 割り込みハンドラ番号を特定	*/
								/* Specifies calling context	*/
	if(0xffff == usRecId){
		return;						/* タイマハンドラへのディスパッチ   */
	}							/* Dispatches to timer handler	*/

	if((bCtxType == _kernel_tps_PrevCtxType) &&		/* 直前に記録したコンテキストと	*/
	   (usRecId  == _kernel_tps_PrevCtxId)){		/* 同一の場合は記録しない	*/
		return;						/* （タイマハンドラの単独実行）	*/
	}							/* Dispatches from timer handler*/

								/* ディスパッチログはCPUロック状態 or	*/
								/* 割り込み禁止状態で呼び出される	*/
								/* The condition when this function	*/
								/* is called is "CPU lock".		*/
	_kernel_tps_DispatchInfo.stDispLog[*pusRecPoint].ulRecTime = current_time;
	_kernel_tps_DispatchInfo.stDispLog[*pusRecPoint].bCtxType  = bCtxType;
	_kernel_tps_DispatchInfo.stDispLog[*pusRecPoint].usCtxId   = usRecId;

	_kernel_tps_PrevCtxType = bCtxType;
	_kernel_tps_PrevCtxId   = usRecId;

	*pusRecPoint += 1;
	if((_kernel_tps_DispatchInfo.stRecCtrl.ulMaxRecCount & 0x0000ffff) <= *pusRecPoint){
		*pusRecPoint = 0;
	}
	if(*pusRecPoint == _kernel_tps_DispatchInfo.stRecCtrl.usRefPoint){
		_kernel_tps_DispatchInfo.stRecCtrl.bOWState = 0x01;
	}

	return;
}


/************************************************************************
 *  tpsRecKObjAccessLog
 *	Type	 : void
 *	Ret val  : none
 *	Argument : unsigned short usObjId ... target object.
 *		   void *pCtxInfo ... ID of the context which calls system call.
 *		   unsigned long  ulDataId ... Data ID
 *		   unsigned long  ulTypesBcd ... includes "kernel object type",
 *				                 "access type", "result" and
 *				                 "context type".
 *				  Assign is in the following.
 *				     Bit  0 -  7 : Kernel object type
 *				     Bit  8 - 15 : Access type
 *				     Bit 16 - 23 : Result
 *				     Bit 24 - 31 : Context type
 *		
 *	Function : Records Kernel object access log with specified parameters.
 ************************************************************************/
void
tpsRecKObjAccessLog(unsigned short usObjId, void *pCtxInfo, unsigned long ulDataId, unsigned long ulTypesBcd)
{
	unsigned char	bKObjType, bAccessType, bResult, bCtxType;
	volatile unsigned short	*pusRecPoint;
	unsigned short		usCtxId;
	int			iRetVal;

	iRetVal = tpsChkRecAreaFull((TPS_LOGREC_CTRL *)&_kernel_tps_KObjAccessInfo,
				    TPS_LOGTYPE_KOBJ);		/* ログ記録の可否をチェック	*/
	if(TPS_E_NG == iRetVal){				/* Checking log area capacity	*/
		return;
	}

	pusRecPoint = &(_kernel_tps_KObjAccessInfo.stRecCtrl.usRecPoint);
	bKObjType   = (unsigned char)(ulTypesBcd         & 0x000000ff);
	bAccessType = (unsigned char)((ulTypesBcd >>  8) & 0x000000ff);
	bResult     = (unsigned char)((ulTypesBcd >> 16) & 0x000000ff);
	bCtxType    = (unsigned char)((ulTypesBcd >> 24) & 0x000000ff);

	usCtxId = tpsSearchCtxId(&bCtxType, pCtxInfo);		/* タスク/周期ハンドラID or	*/
	if(!usCtxId || (0xffff == usCtxId)){			/* 割り込みハンドラ番号を特定	*/
		return;						/* Specify Calling context	*/
	}							/* 想定外のコンテキスト種別	*/
								/* Not supporting context	*/
	if(TPS_OBJTYPE_MBOX != bKObjType &&
	   TPS_OBJTYPE_FLAG != bKObjType){			/* サポート外のオブジェクト種別	*/
		return;						/* Not supporting object type	*/
	}

	{
		SIL_PRE_LOC;					/* (多重)割り込みを禁止するため	*/
		SIL_LOC_INT();					/* IEを操作する			*/
								/* Prohibits interruption	*/
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].ulRecTime   = current_time;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bObjType    = bKObjType;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bAccessType = bAccessType;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].usObjectId  = usObjId;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bResult     = bResult;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].bCtxType    = bCtxType;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].usCtxId     = usCtxId;
		_kernel_tps_KObjAccessInfo.stKObjLog[*pusRecPoint].ulDataId    = ulDataId;

		*pusRecPoint += 1;
		if((_kernel_tps_KObjAccessInfo.stRecCtrl.ulMaxRecCount & 0x0000ffff) <= *pusRecPoint){
			*pusRecPoint = 0;
		}
		if(*pusRecPoint == _kernel_tps_KObjAccessInfo.stRecCtrl.usRefPoint){
			_kernel_tps_KObjAccessInfo.stRecCtrl.bOWState = 0x01;
		}
		SIL_UNL_INT();					/* 割り込み禁止を解除する	*/
	}							/* Allows interruption		*/

	return;
}

