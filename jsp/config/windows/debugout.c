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
 *  @(#) $Id: debugout.c,v 1.10 2003/12/15 07:19:22 takayuki Exp $
 */

#include <debugout.h>
#include <hal_msg.h>
#include <resource.h>

#ifdef DEBUG_ENABLE

#define DBG_SIZE_STRINGBUFFER 512

static HANDLE DebugConsole;     //デバッグ文字列を表示するテキストボックス
static HWND   DialogHandle;     //デバッグ出力ダイアログ

extern void FatalAssertion(int exp, LPCSTR format, ... );

/*
 * デバッグ用ダイアログのメッセージハンドラ
 */
static LRESULT CALLBACK DebugOutDialogProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		DebugConsole = GetDlgItem(hDlg,IDC_DEBUGOUT);       //出力先テキストボックス
		ShowWindow(hDlg,SW_SHOW);

	case WM_SIZE:
		{
			RECT client;
			GetClientRect(hDlg,&client);
			MoveWindow(DebugConsole,0,0,client.right,client.bottom,TRUE);
			break;
		}

	case WM_CLOSE:
			/* Never let this dialog close by pressing of the close button. */
		break;

	case WM_DESTROY:
		DialogHandle = 0;
		break;

	case WM_QUIT:
		DestroyWindow(hDlg);
		break;
	
	default:
		return FALSE;
	}
	return TRUE;
}

    /* 下の2関数が情報をやり取りするための構造体 */
struct tagInitializeDebugServicesParam {
    HINSTANCE hInstance;
    HWND      hDlg;
};

    /* デバッグルーチンをとめる */
void FinalizeDebugServices(void)
{
    if(DialogHandle != 0l) {
		DestroyWindow(DialogHandle);
    }
}

    /* 管理スレッドが実行するウィンドウ作成ルーチン */
static void _initializeDebugServices(void * _param)
{
    struct tagInitializeDebugServicesParam * param = (struct tagInitializeDebugServicesParam *)_param;
    DialogHandle = CreateDialog(param->hInstance, MAKEINTRESOURCE(DEBUGOUTDIALOG), param->hDlg, DebugOutDialogProc);
    FatalAssertion(DialogHandle != NULL, "DebugService could not create its own dialog.");     
}

    /* デバッグルーチンの初期化 （管理スレッドにダイアログを作ってもらう) */
void InitializeDebugServices(HINSTANCE hinst,HWND dlg)
{
    struct tagInitializeDebugServicesParam param;
    param.hInstance = hinst;
    param.hDlg      = dlg;

    HALExecuteProcedure(_initializeDebugServices, (void *)&param);      //生成依頼
    HALAddDestructionProcedure(FinalizeDebugServices, 0);               //破棄ルーチンの登録
}

    /* デバッグ時用printf */
int _debugOut(const char * format, ... )
{
	int result;
	int i;
	char buffer[DBG_SIZE_STRINGBUFFER*2];
	char * scope;
	const char * work;
	va_list vl;

	if(format == 0l)
		return 0;

	va_start(vl, format);

		/* formatに含まれる '\n' を '\r\n' に置換する */
	scope = buffer;
	work  = format;

	while(*format != '\0')
	{
		if(*format == '\n')
		{
			i = format - work;
			lstrcpyn(scope, work, i);
			
			scope += i;
			work = format;

			*(scope++) = '\r';
		}
		format ++;
	}

	i = format - work + 1;
	lstrcpyn(scope, work, i);		//+1して'\x0'もコピー
	scope += i;

	if((result = wvsprintf(scope, buffer, vl)) > 0)
	{
			//出力しすぎたら少しカット
		i = GetWindowTextLength(DebugConsole);
		if(i > DEBUG_LIMITTERUPPERBOUND)
		{
			SendMessage(DebugConsole, EM_SETSEL, 0, i - DEBUG_LIMITTERUPPERBOUND);
			SendMessage(DebugConsole, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)"");
		}

		SendMessage(DebugConsole,EM_SETSEL,i,i);
		SendMessage(DebugConsole,EM_REPLACESEL,(WPARAM)FALSE,(LPARAM)scope);
	}

	return result;
}

#endif


