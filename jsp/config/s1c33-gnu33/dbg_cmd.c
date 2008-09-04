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

#include "jsp_kernel.h"
#include "itron.h"
#include "stdio.h"
#include "string.h"

#include "task.h"
#include "cyclic.h"
#include "mailbox.h"
#include "semaphore.h"
#include "wait.h"
#include "eventflag.h"
#include "dataqueue.h"
#include "mempfix.h"

#define TPS_BUFSIZE	2048		    /* メッセージバッファサイズ     */
					    /* print buffer size (0x0800)   */
#define TPS_EMPTY_MSG	"Warning: Stop by print buffer full."

#define TPS_OK		0		    /* エラーコード */
#define TPS_EXT_PARAM	1
#define TPS_ERROR	-1		    /* error code   */
#define TPS_NO_OBJECT	-2

#define TPS_TSKSTS_RUNDMT   1		    /* タスク状態フラグ */
#define TPS_TSKSTS_SUSPEND  2		    /* Task status flag */

#define TPS_STSID_SYS	0		    /* 状態取得関数用フラグ	    */
#define TPS_STSID_TSK	1		    /* Flags for status reference   */
					    /* function */

#include "kernel_objs.h"
extern SEMCB semcb_table[TNUM_SEMID];
extern FLGCB flgcb_table[TNUM_FLGID];
extern DTQCB dtqcb_table[TNUM_DTQID];
extern MBXCB mbxcb_table[TNUM_MBXID];
extern MPFCB mpfcb_table[TNUM_MPFID];
extern CYCCB cyccb_table[TNUM_CYCID];

static	INT tps_StartNum, tps_EndNum;	    /* 情報表示範囲を保持 */
static	INT tps_TaskFlag;		    /* The start/end number of the  */
					    /* object which displays infor- */
					    /* tion			    */

typedef struct {			    /* メッセージバッファ構造体     */
	INT     iSize;			    /* structure of sprintf buffer  */
	B	    chBuff[TPS_BUFSIZE + 3];
} tps_DebugMsg_t;
static	tps_DebugMsg_t	tps_tmpbuf;

/*
 *  空きバッファサイズのチェック
 *  Check free size of tps_tmpbuf
 */
/************************************************************************
 *  chk_prtbuf
 *	Type	 : INT
 *	Ret val  : Error code
 *		     0 ... Normal exit
 *		    -1 ... Parameter error
 *	Argument : INT	 iChkSize ... Requisite size
 *	Function : Check free buffer size
 ************************************************************************/
static INT chk_prtbuf(INT iChkSize)
{
	INT     iCnt;

	if(iChkSize > (TPS_BUFSIZE - tps_tmpbuf.iSize)){
						    /* 警告メッセージが収まらない場合 */
						    /* when warning message doesn't fit*/
						    /* buffer			      */
		if(tps_tmpbuf.iSize + sizeof(TPS_EMPTY_MSG) + 2 + 1 > TPS_BUFSIZE){
			tps_tmpbuf.iSize = TPS_BUFSIZE - sizeof(TPS_EMPTY_MSG) - 2 - 1;
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%s\n%c",
				TPS_EMPTY_MSG, '\0');

		tps_tmpbuf.iSize += iCnt;

		return TPS_ERROR;
	}
	return TPS_OK;
}

/*
 *  情報出力範囲の設定
 *  Set up the range which display information.
 */
/************************************************************************
 *  chk_param
 *	Type	 : static INT
 *	Ret val  : Return code
 *		    -1 ... Parameter Error
 *		     0 ... all display
 *		     1 ... valid parameters specified
 *	Argument : INT iStart  ... start displaying number.
 *		   INT iEnd    ... terminate displaying number.
 *		   INT iMaxNum ... Max ID number.
 *	Function : Get current IL from PSR.
 ************************************************************************/
static INT chk_param(INT iStart, INT iEnd, INT iMaxNum)
{
	if(iMaxNum == 0){
		return TPS_NO_OBJECT;		    /* 未生成オブジェクトの指定     */
	}					    /* Specified ungenerated object */

	if((iStart < 0) || (iEnd > iMaxNum)){
		return TPS_ERROR;		    /* 異常パラメータ	 */
	}					    /* illegal parameter */

	if(iStart == 0){
		if(iEnd == 0){			    /* 全部表示する		 */
			tps_StartNum = 1;		    /* all display (1 -> maxnum) */
			tps_EndNum = iMaxNum;
			return TPS_OK;
		}
		else{
			return TPS_ERROR;
		}
	}

	if(iEnd > iStart){
		if(iEnd > iMaxNum){
			tps_StartNum = iStart;	    /* 開始値から最大値まで */
			tps_EndNum = iMaxNum;	    /* iStart -> iMaxNum    */
		}else{
			tps_StartNum = iStart;	    /* 開始値から終了値まで */
			tps_EndNum = iEnd;		    /* iStart -> iEnd	    */
		}
	}else{
		if(iStart > iMaxNum){
			return TPS_ERROR;
		}
		tps_StartNum = iStart;		    /* 開始値の単独表示     */
		tps_EndNum = iStart;		    /* iStart only	    */
	}
	return TPS_EXT_PARAM;
}

/*
 *  キューイング情報メッセージ作成処理
 *  Make queuing information
 */
/************************************************************************
 *  print_queue
 *	Type	 : static void
 *	Ret val  : None
 *	Argument : QUEUE *pMember ... the table registering queue top/tail
 *				      address
 *	Function : Make queuing list message.
 ************************************************************************/
static INT print_queue(QUEUE* pMember)
{
	QUEUE* pTskcb;
	INT iFlag, iLoop, iCnt;


	pTskcb = pMember->next;				/* 先頭タスクを取得     */
	iFlag = 0;					/* get Top task-address */

	while(pTskcb != pMember){			/* タスクがあれば検索する */
		iLoop = 0;				/* Search task when Top   */
							/* address is not pMember */
		while((QUEUE*)&tcb_table[iLoop] != pTskcb){
			iLoop++;
			if(iLoop == tmax_tskid){
				break;			/* 見つからなければ終了する */
			}				/* not found(illegal)	*/ 
		}

		if(chk_prtbuf((iFlag) ? 10 : 7)){	/* バッファ容量をチェック */
			return TPS_ERROR;		/* free buffer size check */
		}

		if(iFlag == 0){ 			/* 先頭タスクの表示  */
							/* display head task */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,"task%d", iLoop + 1);
			tps_tmpbuf.iSize += iCnt;
			iFlag = 1;
		}else{					/* 非先頭タスクの表示    */
							/* display continue task */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,"->task%d", iLoop + 1);
			tps_tmpbuf.iSize += iCnt;
		}
		pTskcb = pTskcb->next;			/* 次のタスクを取得      */
	}						/* get next task-address */

	if(iFlag == 0){					/* キューイングタスクなし*/
							/* Queuing no objects    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "-");
		tps_tmpbuf.iSize += iCnt;
	}

	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,"\n");
	tps_tmpbuf.iSize += iCnt;

	return TPS_OK;
}

/*
 *  システム/タスク状態の取得
 *  Refer system/task status.
 */
/************************************************************************
 *  print_status
 *	Type	 : static void
 *	Ret val  : None
 *	Argument : INT	iStsId	  ... target type
 *		   TCB* pstTcb	  ... Task contorl block address
 *		   B*	chStsBuff ... Buffer address
 *	Function : Make status message of system or specified task.
 ************************************************************************/
static void print_status(INT iStsId, TCB* pstTcb, B *chStsBuff)
{
	ER	iRetVal;

	*chStsBuff = '\0';				/* システム状態の取得  */
	if(iStsId == TPS_STSID_SYS){			/* Check System Status */
		if(enadsp == FALSE){
			strcat(chStsBuff, " DDSP");
		}

		iRetVal = sense_lock();
		if(iRetVal == TRUE){
			strcat(chStsBuff, " LOC");
		}

		if(!tps_IntNestCnt){
			strcat(chStsBuff, " TSK");
		}
		else{
			strcat(chStsBuff, " INDP");
		}

		return;
	}

	tps_TaskFlag = 0;
	if(pstTcb == runtsk){				/* タスク状態の取得  */
		sprintf(chStsBuff, "RUN");		/* Check task Status */
		tps_TaskFlag = TPS_TSKSTS_RUNDMT;
	}
	else if(TSTAT_DORMANT(pstTcb->tstat) == TRUE){
		sprintf(chStsBuff, "DMT");
		tps_TaskFlag = TPS_TSKSTS_RUNDMT;
	}
	else if(TSTAT_RUNNABLE(pstTcb->tstat) == TRUE){
		sprintf(chStsBuff, "RDY");
	}
	else if(TSTAT_WAITING(pstTcb->tstat) == TRUE){
		if(TSTAT_SUSPENDED(pstTcb->tstat) == TRUE){
			sprintf(chStsBuff, "WAS");
			tps_TaskFlag = TPS_TSKSTS_SUSPEND;
		}
		else{
			sprintf(chStsBuff, "WAI");
		}
	}
	else if(TSTAT_SUSPENDED(pstTcb->tstat) == TRUE){
		sprintf(chStsBuff, "SUS");
		tps_TaskFlag = TPS_TSKSTS_SUSPEND;
	}

	return;
}

/*
 *  システム状態の表示
 *  Display system status.
 */
/************************************************************************
 *  iprint_mng
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iParam1 ... Reserved(Don't used)
 *		   INT	iParam2 ... Reserved(Don't used)
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make status message of system.
 ************************************************************************/
VP iprint_mng(INT iParam1, INT iParam2, INT iParam3)
{
	INT iCnt;
	B	chCurSts[25];

	tps_tmpbuf.iSize = 0;
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,	/* タイトルの表示 */
			    "System management information\n");	/* print title   */
	tps_tmpbuf.iSize += iCnt;

							/* システム状態の表示*/
	print_status(TPS_STSID_SYS, NULL, chCurSts);    /* print system status */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
			    "System information        (%08xh) :%s\n",
			    (UINT)&enadsp, chCurSts);
	tps_tmpbuf.iSize += iCnt;

							/* タスクIDの表示        */
							/* print running task ID */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, 
			    "Current running task ID   (%08xh) : ",
			    (UINT)&runtsk);
	tps_tmpbuf.iSize += iCnt;
	if(runtsk != NULL){ 	    
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%03d\n",
				TSKID(runtsk));
	}
	else{
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"-\n");
	}
	tps_tmpbuf.iSize += iCnt;

								/* 次タスクIDの表示   */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,	/* print next task ID */
			    "Next task ID              (%08xh) : ",
			    (UINT)&schedtsk);
	tps_tmpbuf.iSize += iCnt;
	if(runtsk != NULL){ 	    
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%03d\n",
				TSKID(schedtsk));
	}
	else{
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"-\n");
	}
	tps_tmpbuf.iSize += iCnt;

								/* システム時間を表示 */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,	/* print current time */
			    "Current time              (%08xh) : %08x\n",
			    (UINT)&current_time, current_time);
	tps_tmpbuf.iSize += iCnt;

							/* 割込みネストカウントの表示    */
							/* print Nesting interrupt count */
	iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
			    "Interrupt nesting count   (%08xh) : %03d\n",
			    (UINT)&tps_IntNestCnt, tps_IntNestCnt);
	tps_tmpbuf.iSize += iCnt;

	tps_tmpbuf.iSize++; 				/* ヌル文字分を加算 */
							/* ++ is null	*/
	return (VP)&tps_tmpbuf;
}

/*
 *  レディキューの表示
 *  Display ready queue.
 */
/************************************************************************
 *  iprint_rdy
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start priority
 *		   INT	iEnd	... End priority
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified ready queue.
 ************************************************************************/
VP iprint_rdy(INT iStart, INT iEnd, INT iParam3)
{
	INT iRetVal, iLoop, iCnt;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, TMAX_TPRI);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

							    /* タイトルを表示	*/
							    /* print title	*/
	iCnt = sprintf(tps_tmpbuf.chBuff,"Ready queue\nPRI  ADDRESS    QUEUE\n");
	tps_tmpbuf.iSize = iCnt;

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(15) != TPS_OK){		/* バッファ容量をチェック */
			break;				/* free buffer size check */
		}

							/* 優先度とアドレスを表示     */
							/* print priority No, address */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"%-3d  %08xh  ",
				iLoop, (UINT)&ready_queue[iLoop-1]);
		tps_tmpbuf.iSize += iCnt;

							/* キューメッセージを作成する */
							/* print queue list */
		print_queue((QUEUE *)&ready_queue[iLoop-1]);
	}

	tps_tmpbuf.iSize ++;				/* ヌル文字分を加算 */
	return (VP)&tps_tmpbuf;				/* ++ is null	*/
}

/*
 *  タスク制御情報の表示
 *  Display task control block.
 */
/************************************************************************
 *  iprint_tsk
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start task ID
 *		   INT	iEnd	... End task ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified ready queue.
 ************************************************************************/
int *iprint_tsk(INT iStart, INT iEnd, INT iParam3)
{
	TCB* pTskcb;
	INT iRetVal, iLoop, iCnt;
	B	chStsBuff[5];

	
	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_tskid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == E_OK){				/* 全部表示する */
							/* all display  */
		iCnt =	sprintf(tps_tmpbuf.chBuff, "Task overview\n");
		iCnt += sprintf(tps_tmpbuf.chBuff + iCnt,
				"TID  PC         SP         STA\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(32) != TPS_OK){	/* バッファ容量をチェック */
				break;			/* free buffer size       */
			}

			pTskcb = (TCB*)&tcb_table[iLoop-1];
			print_status(TPS_STSID_TSK, pTskcb, chStsBuff);
					    
			if(tps_TaskFlag == 1){		/* タスクが休止, 実行状態の場合 */
							/* case DMT,RUN 		*/
				iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"%-3d  --------   --------   %3s\n",
					iLoop, chStsBuff);
				tps_tmpbuf.iSize += iCnt;
			}else{
				iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"%-3d  %08xh  %08xh  %3s\n",
					iLoop, (UINT)pTskcb->tskctxb.pc,
					(UINT)pTskcb->tskctxb.sp, chStsBuff);
				tps_tmpbuf.iSize += iCnt;
			}
		}
		tps_tmpbuf.iSize++;			/* ヌル文字分を加算 */
							/* ++ is nul	    */
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(12) != TPS_OK){
			break;
		}
							/* タスクIDを表示	*/
							/* print task ID	*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"TSKID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		pTskcb = (TCB*)&tcb_table[iLoop-1];
		if(chk_prtbuf(32) != E_OK){
			break;
		}
		print_status(TPS_STSID_TSK, pTskcb, chStsBuff);
							/* タスク制御ブロックアドレスを表示 */
							/* print taskcb address 	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Taskcb addr        : %08xh\n",
				(UINT)pTskcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							/* 現在のPCを表示   */
							/* print current PC */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"PC                 : ");
		tps_tmpbuf.iSize += iCnt;
		if(tps_TaskFlag == 1){			/* タスクが休止, 実行状態の場合 */
							/* case DMT, RUN	        */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "--------\n");
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%08xh\n",
				    (UINT)pTskcb->tskctxb.pc);
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							    /* PC初期値を表示する */
							    /* print initial PC   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Initial PC         : %08xh\n",
				(UINT)pTskcb->tinib->task);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							/* 現在のSPを表示   */
							/* print current SP */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"SP                 : ");
		tps_tmpbuf.iSize += iCnt;
		if(tps_TaskFlag == 1){			/* タスクが休止, 実行状態の場合 */
							/* case DMT, RUN	        */
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "--------\n");
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%08xh\n",
				    (UINT)pTskcb->tskctxb.sp);
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != E_OK){
			break;
		}
							/* SP初期値を表示する */
							/* print initial SP   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Initial SP         : %08xh\n",
				(UINT)pTskcb->tinib->stk + pTskcb->tinib->stksz);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(25) != E_OK){
			break;
		}
							/* 現在の優先度を表示する */
							/* print current priority */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Priority           : %-2d\n",
				pTskcb->priority + TMIN_TPRI);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(25) != E_OK){
			break;
		}
							/* 優先度の初期値を表示する */
							/* print initial priority   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Initial priority   : %-2d\n",
				pTskcb->tinib->ipriority + TMIN_TPRI);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(26) != E_OK){
			break;
		}
							/* タスク状態を表示する */
							/* print task status    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Task status        : %3s\n",
				chStsBuff);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != E_OK){
			break;
		}
							/* 起動要求数を表示する      */
							/* print Start request count */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Start Req.   count : %d\n",
				pTskcb->actcnt);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != E_OK){				
			break;
		}
							/* 起床要求数を表示する       */
							/* print wake up request count*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"WUP Req.     count : %d\n",
				pTskcb->wupcnt);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != E_OK){
			break;
		}
							/* サスペンド要求数を表示する */
							/* print Suspend request count*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Suspend Req. count : %d\n\n",
				(tps_TaskFlag == TPS_TSKSTS_SUSPEND) ? 1 : 0);
		tps_tmpbuf.iSize += iCnt;
	}

	tps_tmpbuf.iSize++;
	return((VP)&tps_tmpbuf);
}

/*
 *  周期ハンドラ制御情報の表示
 *  Display cyclic handler control block.
 */
/************************************************************************
 *  iprint_cyc
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler No.
 *		   INT	iEnd	... End handler No.
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified ready queue.
 ************************************************************************/
VP iprint_cyc(INT iStart, INT iEnd, INT Param3)
{
	CYCCB*  pHndrcb;
	INT     iRetVal, iLoop, iCnt;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_cycid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* 全部表示する */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Cyclic handler overview\n");
		tps_tmpbuf.iSize = iCnt;
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"NO   ACT    PC         CYCLETIME  ENTRYTIME\n");
		tps_tmpbuf.iSize += iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(45) != TPS_OK){
				break;
			}
							/* ハンドラ状態を表示する */
							/* print handler status   */
			pHndrcb = (CYCCB*)&cyccb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %-3s    %08xh  %08xh  %08xh\n",
				    iLoop, ((pHndrcb->cycsta == TRUE) ? "ON" : "OFF"),
				    (UINT)pHndrcb->cycinib->cychdr,
				    pHndrcb->cycinib->cyctim, pHndrcb->evttim);
			tps_tmpbuf.iSize += iCnt;
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(26) != TPS_OK){
			break;
		}
							/* ハンドラ番号を表示する */
							/* print handler No.      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Cyclic handler No. = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/*周期ハンドラ制御ブロックアドレスを表示する */
							/* print cyclic handler cb address	     */
		pHndrcb = (CYCCB*)&cyccb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Cyccb addr       : %08xh\n",
				(UINT)pHndrcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != TPS_OK){
			break;
		}
							/* 活性状態を表示する */
							/* print activation   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Activation       : %-3s\n",
				(pHndrcb->cycsta == TRUE) ? "ON" : "OFF");
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* 拡張情報格納領域を表示する 		  */
							/* print address stored expand information*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Expand Info addr : %08xh\n",
				(UINT)pHndrcb->cycinib->exinf);
		tps_tmpbuf.iSize += iCnt;
		
		if(chk_prtbuf(30) != TPS_OK){			    
			break;
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, /* PCを表示する */
				"PC               : %08xh\n",	     /* print PC     */
				(UINT)pHndrcb->cycinib->cychdr);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* 位相時間を表示する */
							/* print phase time   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Phase time       : %08xh\n",
				pHndrcb->cycinib->cycphs);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* 周期時間を表示する */
							/* print cycle time   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Cycle time       : %08xh\n",
				pHndrcb->cycinib->cyctim);
		tps_tmpbuf.iSize += iCnt;
		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* 起動時刻を表示する */
							/* print entry time   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Entry time       : %08xh\n\n",
				pHndrcb->evttim);
		tps_tmpbuf.iSize += iCnt;
	}

	tps_tmpbuf.iSize ++;
	return (VP)&tps_tmpbuf;
}

/*
 *  メールボックス制御情報の表示
 *  Display mail box control block.
 */
/************************************************************************
 *  iprint_mbx
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified mail box.
 ************************************************************************/
VP iprint_mbx(INT iStart, INT iEnd, INT iParam3)
{
	MBXCB*  pMlbxcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    chAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_mbxid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* 全部表示する */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Mail box overview\nID   MSG  QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(11) != TPS_OK){
				break;
			}
			pMlbxcb = (MBXCB*)&mbxcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %3s  ",
				    iLoop, (pMlbxcb->head == NULL) ? " - " : " * ");
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pMlbxcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(20) != TPS_OK){
			break;
		}
							/* メールボックスIDを表示する */
							/* print mail box ID	      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mail box ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* メールボックス制御ブロックアドレス */
							/* を表示する			      */
							/* print control box address	      */
		pMlbxcb = (MBXCB *)&mbxcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mbxcb addr         : %08xh\n",
				(UINT)pMlbxcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* メールボックス属性を表示する */
							/* print mail box attribute     */
		if(pMlbxcb->mbxinib->mbxatr == (TA_TFIFO | TA_MFIFO)){
			chAtrMsg = "TA_TFIFO | TA_MFIFO";
		}
		else if(pMlbxcb->mbxinib->mbxatr == (TA_TFIFO | TA_MPRI)){
			chAtrMsg = "TA_TFIFO | TA_MPRI";
		}
		else if(pMlbxcb->mbxinib->mbxatr == (TA_TPRI | TA_MFIFO)){
			chAtrMsg = "TA_TPRI | TA_MFIFO";
		}
		else{
			chAtrMsg = "TA_TPRI | TA_MPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mail box attribute : %s\n",
				chAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* 最大優先度を表示する */
							/* print Max priority   */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Max mail priority  : %08xh\n",
				pMlbxcb->mbxinib->maxmpri);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(28) != TPS_OK){
			break;
		}
							/* メールの有無を表示する */
							/* print mail exist/empty */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Message list       : %s\n",
				(pMlbxcb->head == NULL) ? "Empty" : "Exist");
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* 受信待ちキューを表示する */
							/* Print waitting queue     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Rcv queue          : ");
		tps_tmpbuf.iSize += iCnt;

		iRetVal = print_queue((QUEUE*)pMlbxcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  セマフォ制御情報の表示
 *  Display semaphore control block.
 */
/************************************************************************
 *  iprint_sem
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified semaphore.
 ************************************************************************/
VP iprint_sem(INT iStart, INT iEnd, INT iParam3)
{
	SEMCB*  pSemcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    pchAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_semid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* 全部表示する */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Semaphore overview\nID   MAX  CNT  QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(16) != TPS_OK){
				break;
			}
			pSemcb = (SEMCB*)&semcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %-3d  %-3d  ",
				    iLoop, pSemcb->seminib->maxsem, pSemcb->semcnt);
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pSemcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(20) != TPS_OK){		    
			break;
		}
							/* セマフォIDを表示する */
							/* print mail box ID    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semaphore ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* セマフォ制御ブロックアドレスを表示する */
							/* print control box address		  */
		pSemcb = (SEMCB*)&semcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semcb addr          : %08xh\n",
				(UINT)pSemcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(30) != TPS_OK){
			break;
		}
							/* セマフォ属性を表示する    */
							/* print semaphore attribute */
		if(pSemcb->seminib->sematr == TA_TFIFO){
			pchAtrMsg = "TA_TFIFO";
		}
		else{
			pchAtrMsg = "TA_TPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semaphore attribute : %s\n",
				pchAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* セマフォカウント最大値を表示する */
							/* print Max count		    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Max semaphore count : %08xh\n",
				pSemcb->seminib->maxsem);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
}							/* 現在のセマフォカウントを表示する */
							/* print current semaphore count    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Semaphore count     : %08xh\n",
				pSemcb->semcnt);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != TPS_OK){
			break;
		}
							/* 取得待ちタスクのキューを表示する */
							/* print Waitting task queue	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait queue          : ");
		tps_tmpbuf.iSize += iCnt;

		iRetVal = print_queue((QUEUE*)pSemcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  イベントフラグ制御情報の表示
 *  Display event flag control block.
 */
/************************************************************************
 *  iprint_flg
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified event flag.
 ************************************************************************/
VP iprint_flg(INT iStart, INT iEnd, INT iParam3)
{
	FLGCB	*pFlgcb;
	TCB 	*pTskcb;
	WINFO_FLG	*pWaitFlgInfo;
	INT     iRetVal, iLoop, iCnt;
	B	    chAtrMsg[30];

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_flgid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* 全部表示する */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Event flag overview\nID   QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(6) != TPS_OK){
				break;
			}
			pFlgcb = (FLGCB*)&flgcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  ", iLoop);
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pFlgcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* イベントフラグIDを表示する */
							/* print event flag ID	      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Event flag ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* イベントフラグ制御ブロック */
							/* アドレスを表示する	      */
							/* print control box address  */
		pFlgcb = (FLGCB*)&flgcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Flgcb addr           : %08xh\n",
				(UINT)pFlgcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(58) != TPS_OK){
			break;
		}
							/* イベントフラグ属性を表示する */
							/* print event flag attribute   */
		chAtrMsg[0] = '\0';
							/* タスク優先度によるキューイング */
							/* queuing by task priority       */
		if(pFlgcb->flginib->flgatr & TA_TPRI){
			strcat(chAtrMsg, "TA_TPRI");
		}
		else{
			strcat(chAtrMsg, "TA_TFIFO");	/* FIFOでのキューイング */
		}					/* queuing by FIFO	*/

		if(pFlgcb->flginib->flgatr & 0x02){
			strcat(chAtrMsg, " | TA_WMUL");
		}
		else{
			strcat(chAtrMsg, " | TA_WSGL");
		}
							/* パターン一致後フラグのクリア */
							/* cleaing flag pattern	        */
		if(pFlgcb->flginib->flgatr & TA_CLR){
			strcat(chAtrMsg, " | TA_CLR");	
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Event flag attribute : %s\n",
				chAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* 現在のフラグパターンを表示する */
							/* print current flag pattern     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Flag pattern         : %08xh\n",
				pFlgcb->flgptn);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* 待ちタスクのフラグパターン	      */
							/* print wait flag pattern of top task*/
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait flag pattern    : ");
		tps_tmpbuf.iSize += iCnt;
		
		pTskcb = (TCB *)pFlgcb->wait_queue.next;
		pWaitFlgInfo = (WINFO_FLG*)pTskcb->winfo;
		if(pTskcb != (TCB *)&pFlgcb->wait_queue){
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%08xh\n",
				    pWaitFlgInfo->waiptn);
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "-\n");
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* 待ちモードの表示 */
							/* print wait mode  */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"Wait mode            : ");
		tps_tmpbuf.iSize += iCnt;
		if(pTskcb != (TCB *)&pFlgcb->wait_queue){
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"%s\n", (pWaitFlgInfo->wfmode == TWF_ORW) ? "TWF_ORW" : "TWF_ANDW");
		}
		else{
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
					"-\n");
		}
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(24) != TPS_OK){
			break;
		}
							/* 取得待ちタスクのキューを表示する */
							/* print Waitting task queue	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait queue           : ");
		tps_tmpbuf.iSize += iCnt;

		iRetVal = print_queue((QUEUE*)pFlgcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  データキュー制御情報の表示
 *  Display data queue control block.
 */
/************************************************************************
 *  iprint_dtq
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified data queue.
 ************************************************************************/
VP iprint_dtq(INT iStart, INT iEnd, INT iParam3)
{
	DTQCB*  pDtqcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    pchAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_dtqid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* 全部表示する */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Data queue overview\nID   MSG       QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(16) != TPS_OK){
				break;
			}
							/* 受信状態を表示する */
							/* print recieve status */
			pDtqcb = (DTQCB*)&dtqcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  %s  RCV  ",
				    iLoop, (pDtqcb->count == 0) ? " - " : " * ");
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)&pDtqcb->rwait_queue);

			if(chk_prtbuf(16) != TPS_OK){
				break;
			}
							/* 送信状態を表示する */
							/* print send status  */
			pDtqcb = (DTQCB*)&dtqcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "          SND  ");
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)&pDtqcb->swait_queue);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* データキューIDを表示する */
							/* print Data queue ID	    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Data queue ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(34) != TPS_OK){
			break;
		}
							/* データキュー制御ブロック */
							/* アドレスを表示する	    */
							/* print control box address*/
		pDtqcb = (DTQCB*)&dtqcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Dtqcb addr                  : %08xh\n",
				(UINT)pDtqcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(40) != TPS_OK){
			break;
		}
							/* データキュー属性を表示する */
							/* print data queue attribute */
		if(pDtqcb->dtqinib->dtqatr == TA_TFIFO){
			pchAtrMsg = "TA_TFIFO";
		}
		else{
			pchAtrMsg = "TA_TPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Data queue attribute        : %s\n",
				pchAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* キュー領域の先頭アドレスを表示する   */
							/* print top address of data queue area */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Data queue area top address : %08xh\n",
				(UINT)pDtqcb->dtqinib->dtq);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* データ記録位置を表示する      */
							/* print current offset from top */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Top data offset             : %08xh\n",
				pDtqcb->head);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(42) != TPS_OK){
			break;
		}
							/* キューイングデータ数を表示するする */
							/* print queuing data count	      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Queuing data count          : %08xh\n",
				pDtqcb->count);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* 受信待ちタスクキューを表示する     */
							/* print Waitting task queue(recieve) */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Rcv task queue              : ");
		tps_tmpbuf.iSize += iCnt;
		iRetVal = print_queue((QUEUE*)&pDtqcb->rwait_queue);
		if(iRetVal != TPS_OK){
			break;
		}

		if(chk_prtbuf(32) != TPS_OK){
			break;
		}
							/* 送信待ちタスクキューを表示する  */
							/* print Waitting task queue(send) */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Snd task queue              : ");
		tps_tmpbuf.iSize += iCnt;
		iRetVal = print_queue((QUEUE*)&pDtqcb->swait_queue);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}
	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

/*
 *  固定長メモリプール制御情報の表示
 *  Display fixed memory pool control block.
 */
/************************************************************************
 *  iprint_mpf
 *	Type	 : VP
 *	Ret val  : Address of debug message buffer.
 *	Argument : INT	iStart	... Start handler ID
 *		   INT	iEnd	... End handler ID
 *		   INT	iParam3 ... Reserved(Don't used)
 *	Function : Make debug message of specified memory pool.
 ************************************************************************/
VP iprint_mpf(INT iStart, INT iEnd, INT iParam3)
{
	MPFCB*  pMpfcb;
	INT     iRetVal, iLoop, iCnt;
	B*	    pchAtrMsg;

	tps_tmpbuf.iSize = 0;
	iRetVal = chk_param(iStart, iEnd, tmax_mpfid);
	if(iRetVal == TPS_ERROR){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Parameter Error\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}
	else if(iRetVal == TPS_NO_OBJECT){
		iCnt = sprintf(tps_tmpbuf.chBuff, "Specified object is not generated\n");
		tps_tmpbuf.iSize = iCnt + 1;
		return (VP)&tps_tmpbuf;
	}

	if(iRetVal == TPS_OK){				/* 全部表示する */
							/* all display  */
		iCnt = sprintf(tps_tmpbuf.chBuff, "Memory pool overview\nID   QUEUE\n");
		tps_tmpbuf.iSize = iCnt;

		for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
			if(chk_prtbuf(6) != TPS_OK){
			break;
			}
			pMpfcb = (MPFCB*)&mpfcb_table[iLoop - 1];
			iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				    "%-3d  ",
				    iLoop);
			tps_tmpbuf.iSize += iCnt;
			print_queue((QUEUE*)pMpfcb);
		}
		tps_tmpbuf.iSize++;
		return (VP)&tps_tmpbuf;
	}

	tps_tmpbuf.iSize = 0;
	for(iLoop = tps_StartNum; iLoop <= tps_EndNum; iLoop++){
		if(chk_prtbuf(22) != TPS_OK){
			break;
		}
							/* メモリプールIDを表示する */
							/* print memory pool ID     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool ID = %-3d\n",
				iLoop);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* メモリプール制御ブロック */
							/* アドレスを表示する	    */
							/* print control box address*/
		pMpfcb = (MPFCB*)&mpfcb_table[iLoop - 1];
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Mpfcb addr                      : %08xh\n",
				(UINT)pMpfcb);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(44) != TPS_OK){
			break;
		}
							/* メモリプール属性を表示する */
							/* print data queue attribute */
		if(pMpfcb->mpfinib->mpfatr == TA_TFIFO){
			pchAtrMsg = "TA_TFIFO";
		}
		else{
			pchAtrMsg = "TA_TPRI";
		}
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool attribute           : %s\n",
				pchAtrMsg);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* ブロックサイズを表示する */
							/* print memory block size  */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory block size               : %08xh\n",
				pMpfcb->mpfinib->blksz);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* メモリプールの先頭アドレスを表示する */
							/* print top address of memory pool     */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool area top address    : %08xh\n",
				(UINT)pMpfcb->mpfinib->mpf);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* メモリプールの末尾アドレスを表示する */
							/* print tail address of memory pool    */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Memory pool area tail address   : %08xh\n",
				(UINT)pMpfcb->mpfinib->limit);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(46) != TPS_OK){
			break;
		}
							/* 未使用領域の先頭アドレスを表示する */
							/* print top area of unused area      */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Unused area top address         : %08xh\n",
				(UINT)pMpfcb->unused);
		tps_tmpbuf.iSize += iCnt;

		if(chk_prtbuf(48) != TPS_OK){				
			break;
		}
							/* 返却ブロックリストトップのブロック    */
							/* アドレスを表示する		     */
							/* print top address released block list */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Released block list top address : %08xh\n",
				(UINT)pMpfcb->freelist);
		tps_tmpbuf.iSize += iCnt;

							/* 取得待ちタスクのキューを表示する */
							/* print Waitting task queue(send)  */
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize,
				"Wait task queue                 : ");
		tps_tmpbuf.iSize += iCnt;
		iRetVal = print_queue((QUEUE*)pMpfcb);
		iCnt = sprintf(tps_tmpbuf.chBuff + tps_tmpbuf.iSize, "\n");
		tps_tmpbuf.iSize += iCnt;
		if(iRetVal != TPS_OK){
			break;
		}

	}

	tps_tmpbuf.iSize++;
	return (VP)&tps_tmpbuf;
}

