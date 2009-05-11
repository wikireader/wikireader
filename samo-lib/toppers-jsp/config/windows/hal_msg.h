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
 *  @(#) $Id: hal_msg.h,v 1.14 2003/12/15 07:19:22 takayuki Exp $
 */


#ifndef __HAL_MSG_H
#define __HAL_MSG_H

#include <constants.h>
#include <t_services.h>
#include <vwindows.h>


#ifndef __HAL_MSG_MSGONLY

    /* OSの動作を管理するスレッドのハンドル */
extern HWND PrimaryDialogHandle;

    /* 管理スレッドをロックさせないためのSendMessage */
Inline void HALSendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(PrimaryDialogHandle != GetCurrentThread())
        SendMessage(PrimaryDialogHandle, uMsg, wParam, lParam);
    else
        PostMessage(PrimaryDialogHandle, uMsg, wParam, lParam);
}

    //ディスパッチ依頼
Inline void
HALDispatchRequest(void)
{   HALSendMessage(HALMSG_MESSAGE, HALMSG_DISPATCH, (LPARAM)0);   }

    //コンテキスト破棄依頼 (注 : inlineだが、これを参照している場所はひとつだけなのでそのままにする)
Inline void
HALDestroyRequest(void * tcb)
{	PostMessage(PrimaryDialogHandle,HALMSG_MESSAGE,HALMSG_DESTROY,(LPARAM)tcb);   }

    //割込み依頼 (intno : 割込み番号)
Inline void
HALInterruptRequest(unsigned int intno)
{	PostMessage(PrimaryDialogHandle,HALMSG_MESSAGE,HALMSG_INTERRUPT,(LPARAM)intno);   }

    //割込み処理完了通知
Inline void
HALInterruptRequestAndWait(void)
{
	SendMessage(PrimaryDialogHandle,HALMSG_MESSAGE,HALMSG_INTERRUPT_FINISH,(LPARAM)0);
}

    //任意のルーチンの実行要求
Inline BOOL
HALExecuteProcedure(void * func, void * param)
{
	void * _workofHALExecuteProcedure[2];
	
	if(func == NULL)
		return FALSE;

	_workofHALExecuteProcedure[0] = func;
	_workofHALExecuteProcedure[1] = param;
	HALSendMessage(HALMSG_MESSAGE,HALMSG_EXECUTEPROCEDURE,(LPARAM)_workofHALExecuteProcedure);

	return TRUE;
}

    //破棄チェーンに関数を登録
Inline BOOL
HALAddDestructionProcedure(void * func, void * param)
{
	void * _workofHALAddDestructionProcedure[2];

	if(func == NULL)
		return FALSE;

	_workofHALAddDestructionProcedure[0] = func;
	_workofHALAddDestructionProcedure[1] = param;
    HALSendMessage(HALMSG_MESSAGE,HALMSG_ADDDESTRUCTIONPROCEDURE,(LPARAM)_workofHALAddDestructionProcedure);

	return TRUE;
}

    //カーネル動作の終了要求
Inline void
HALQuitRequest(void)
{
    HALSendMessage(HALMSG_MESSAGE,HALMSG_QUITREQUEST,0);
}

#endif

#endif

/***/