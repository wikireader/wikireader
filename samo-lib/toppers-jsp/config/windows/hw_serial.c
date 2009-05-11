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
 *  @(#) $Id: hw_serial.c,v 1.13 2003/12/15 07:19:22 takayuki Exp $
 */



#include <hw_serial.h>
#include <hal_msg.h>
#include <t_services.h>

#include <resource.h>
#include <debugout.h>

#define BUFSZ_UPPERBOUND	24*1024	/* コンソールバッファの巻戻し基準サイズ (これを超えると巻き戻し)  (どうやら30000を超えられないらしい)*/
#define BUFSZ_LOWERBOUND	 8*1024	/* 巻き戻したときにどのくらい巻き戻すか (UPPERBOUNDよりも小さい数)*/

#define ID_PORT(x)		((x) + 1)
#define INDEX_PORT(x)	((x) - 1)
#define GET_SIOPCB(x)	(&siopcb_table[INDEX_PORT(x)])

#define BITTEST(x,y)	( ( (x) & (y) )	!= 0)
#define BITSET(x,y)		InterlockedExchange( &(x), (x) | (y)  )
#define BITCLEAR(x,y)	InterlockedExchange( &(x), (x) & ~(y) )

extern HINSTANCE	ProcessInstance;
extern HANDLE		PrimaryThreadHandle;

	/* シリアル制御ブロック */
SIOPCB siopcb_table[TNUM_PORT];


	/* 致命的なエラー発生時用アサート */
extern void FatalAssertion(int exp, LPCSTR format, ... );


/*
 *  シリアルI/O共通部インタフェース
 */

/*===========================================================================*/

/*
 * コンソール型シリアル
 */

#define MAX_CONSOLE_BUFSZ	2048
#define SERMSG_CREATE		WM_APP
#define SERMSG_UPDATE		(WM_APP+1)

struct tagSerialConsoleParameters
{
	CRITICAL_SECTION cs;
	unsigned int     position;
	char             buffer[MAX_CONSOLE_BUFSZ];
};


static LRESULT CALLBACK KeyEventTrapper(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	SIOPCB * scope;
	
		/* WM_CHARをフックする */
	if(Msg == WM_CHAR)
	{
		scope = (SIOPCB *)GetWindowLong(GetParent(hWnd),GWL_USERDATA);
		PostMessage(scope->Handle,WM_CHAR,wParam,lParam);
		return TRUE;
	}

		/* もともとのWndProcを呼びなおす */
	return CallWindowProc((void *)GetWindowLong(hWnd,GWL_USERDATA),hWnd,Msg,wParam,lParam);
}

static void SelectConsoleFont(HWND console, UINT pixel)
{
	HANDLE  oldfont;
	HANDLE  newfont;
	LOGFONT logfont;
	HDC     hDC;

		/* DC取得 */
	hDC = GetDC(console);

		/* 今のフォント情報を取得する */
	oldfont = (HANDLE) SendMessage(console, WM_GETFONT, 0, 0);
	GetObject(oldfont, sizeof(LOGFONT), &logfont);
	
		/* ポイントを変更 */
	logfont.lfHeight = -MulDiv(pixel, GetDeviceCaps(hDC, LOGPIXELSY), 72);

		/* 更新したフォント情報を元に、新しいフォントを生成して選択 */
	newfont = CreateFontIndirect(&logfont);
	SendMessage(console, WM_SETFONT, (WPARAM)newfont, MAKELPARAM(TRUE,0));

		/* 前のフォントを破棄 */
	DeleteObject(oldfont);

		/* DC解放 */
	ReleaseDC(console, hDC);
}

static LRESULT ConsoleCommandHandler(HWND hDlg, UINT wID, UINT wNotifyCode, LPARAM lParam)
{
	BOOL     result;
	HANDLE   console;

	result  = TRUE;
	console = GetDlgItem(hDlg, IDC_CONSOLE);

	switch(wID)
	{
		/*
		 * フォントの大きさを変更する 
		 */
	case ID_FONT_BIG:
		SelectConsoleFont(console, 16);
		break;
	case ID_FONT_NORMAL:
		SelectConsoleFont(console, 9);
		break;
	case ID_FONT_SMALL:
		SelectConsoleFont(console, 4);
		break;

	default:
		result = FALSE;
	}

	return result;
}

static BOOL CALLBACK ConsoleProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case SERMSG_CREATE:
		{
			SIOPCB * work;
			void   * DefWndProc;
			HWND     hConsole;

			hConsole = GetDlgItem(hDlg, IDC_CONSOLE);

				/* テキストボックスのWndProcにフックをかける */
			DefWndProc = (void *)GetWindowLong(hConsole,GWL_WNDPROC);
			SetWindowLong(hConsole, GWL_USERDATA, (LONG) DefWndProc);

			work = (SIOPCB *)lParam;
			SetWindowLong(hDlg,GWL_USERDATA,lParam);
			SetWindowLong(hConsole, GWL_WNDPROC, (long)KeyEventTrapper);

			SetTimer(hDlg, 100, 300, NULL);

			ShowWindow(hDlg,SW_SHOWNA);
			break;
		}

	case WM_DESTROY:
		{
			SIOPCB * scope;
			KillTimer(hDlg, 100);
			scope = (SIOPCB *)GetWindowLong(hDlg,GWL_USERDATA);
			if(scope != 0)
				scope->Handle = NULL;
			break;
		}

	case WM_CLOSE:	/* ユーザによるウィンドウクローズを抑止 */
		break;

	case WM_CHAR:
		{
			SIOPCB * scope;
			scope = (SIOPCB *)GetWindowLong(hDlg,GWL_USERDATA);
			if(scope != 0)
			{
				scope->ReceiveBuffer = (char)wParam;
				BITSET(scope->Flag, SIO_STA_INTRCV);
				HALInterruptRequest(INHNO_SERIAL);
			}
			break;
		}

	case WM_SETFONT:
		return TRUE;

	case WM_INITDIALOG:
    case WM_SIZE:
		{
			RECT client;
			GetClientRect(hDlg,&client);
			MoveWindow(GetDlgItem(hDlg,IDC_CONSOLE),0,0,client.right,client.bottom,TRUE);
			break;
		}

	case WM_COMMAND:
		return ConsoleCommandHandler(hDlg, LOWORD(wParam), HIWORD(wParam), lParam);

		//一定時間たったら改行がこなくても出力する
	case WM_TIMER:
		if(wParam == 100)
		{
			SIOPCB * scope;
			struct tagSerialConsoleParameters * param;

			scope = (SIOPCB *)GetWindowLong(hDlg, GWL_USERDATA);
            if(scope != 0) {
			    param = (struct tagSerialConsoleParameters *)scope->versatile;
			    if(param->position == 0)
				    break;

			    lParam = TRUE;
            }
            else
                break;  //まだ初期化が終わってないので何もしない
		}

		//lParam : 送信が終わった後に割込みをかけるかどうか (FALSE:かけない TRUE:かける)
	case SERMSG_UPDATE:
		{
			LRESULT  result;
			int		 textlength;
			HANDLE	 console;
			SIOPCB * scope;
			struct tagSerialConsoleParameters * param;

			scope = (SIOPCB *)GetWindowLong(hDlg, GWL_USERDATA);
            if(scope != 0) {
			    param = (struct tagSerialConsoleParameters *)scope->versatile;

                console = GetDlgItem(hDlg, IDC_CONSOLE);

			    textlength = GetWindowTextLength(console);
			    if(textlength > BUFSZ_UPPERBOUND)
			    {
					    /* 古い情報を消す */
				    SendMessage(console,EM_SETSEL,0,textlength - BUFSZ_LOWERBOUND);
				    SendMessage(console,EM_REPLACESEL,(WPARAM)FALSE,(LPARAM)"");
				    textlength = GetWindowTextLength(console);
			    }
				    /* 末尾に文字を置く */
			    result = SendMessage(console,EM_SETSEL,textlength,textlength);

			    EnterCriticalSection(&param->cs);
			    param->buffer[param->position] = '\x0';
			    result = SendMessage(console,EM_REPLACESEL,(WPARAM)FALSE,(LPARAM)param->buffer);
			    param->position = 0;
			    LeaveCriticalSection(&param->cs);

			    if(lParam == TRUE)
			    {
				    BITSET(scope->Flag, SIO_STA_INTSND);
				    HALInterruptRequest(INHNO_SERIAL);
			    }
            }
            else
                PostMessage(hDlg, Msg, wParam, lParam);     //まだ初期化が終わってないので末尾につけなおす

			break;
		}

	default:
		return FALSE;
	}

	return TRUE;
}

static void SerialConsole_FinalRelease(void * param)
{
	SIOPCB * cb = (SIOPCB *)param;
	struct tagSerialConsoleParameters * versatile;

	if(cb->Handle != 0l && cb->Handle != NULL)
		DestroyWindow(cb->Handle);
	cb->Handle = NULL;

	versatile = cb->versatile;
	DeleteCriticalSection(&versatile->cs);
	GlobalFree((HGLOBAL)versatile);
}

static void CreateSerialConsole(SIOPCB * cb)
{
	struct tagSerialConsoleParameters * param;

	param = GlobalAlloc(GMEM_FIXED, sizeof(struct tagSerialConsoleParameters));
	FatalAssertion(param != NULL, "CreateSerialConsole: GlobalAlloc reported NULL.");
	param->position = 0;
	InitializeCriticalSection(&param->cs);
	cb->versatile = param;

	cb->Handle = CreateDialog(ProcessInstance, MAKEINTRESOURCE(CONSOLEDIALOG), 0, ConsoleProc);
	FatalAssertion(cb->Handle != 0, "CreateSerialConsole could not create its dialog.");
	PostMessage(cb->Handle,SERMSG_CREATE,0,(LPARAM)cb);
	UpdateWindow(cb->Handle);
	HALAddDestructionProcedure(SerialConsole_FinalRelease,cb);
}

static BOOL SerialConsole_PutChar(SIOPCB * cb, INT chr, BOOL rasint)
{
	char	buffer[2];
	int		textlength;
	HANDLE	console;

	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_CONSOLE));
	assert(cb->Handle != NULL);

	console = GetDlgItem(cb->Handle, IDC_CONSOLE);
	buffer[0] = (char) chr;
	buffer[1] = '\x0';

	textlength = GetWindowTextLength(console);
	if(textlength > BUFSZ_UPPERBOUND)
	{
			/* 古い情報を消す */
		SendMessage(console,EM_SETSEL,0,textlength - BUFSZ_LOWERBOUND);
		SendMessage(console,EM_REPLACESEL,(WPARAM)FALSE,(LPARAM)"");
		textlength = GetWindowTextLength(console);
	}
		/* 末尾に文字を置く */
	SendMessage(console,EM_SETSEL,textlength,textlength);
	SendMessage(console,EM_REPLACESEL,(WPARAM)FALSE,(LPARAM)buffer);

	if(rasint == TRUE)
	{
		BITSET(cb->Flag, SIO_STA_INTSND);
		HALInterruptRequest(INHNO_SERIAL);
	}

	return TRUE;
}

static BOOL SerialConsole_PushChar(SIOPCB * cb, INT chr)
{
	BOOL success;
	BOOL update;
	struct tagSerialConsoleParameters * param;

	param = (struct tagSerialConsoleParameters *)cb->versatile;

	do {
		success = FALSE;
		update  = FALSE;

		EnterCriticalSection(&param->cs);

		if(param->position < MAX_CONSOLE_BUFSZ)
		{
			param->buffer[param->position] = (char)chr;
			param->position ++;

			if(chr == '\n')
				update = TRUE;
		}else
			update = TRUE;

		LeaveCriticalSection(&param->cs);

		if(update == TRUE)
			SendMessage(cb->Handle, SERMSG_UPDATE, 0, 1);
	} while(success == TRUE);

	return TRUE;
}

/*===========================================================================*/

/*
 *   Windowsのコンソールを使用したシリアル入出力
 */

static DWORD WINAPI WinConsole_ReceiverThread(LPVOID param)
{
	SIOPCB *		cb = (SIOPCB *)param;
	DWORD			work;
	HANDLE			stdin;
	INPUT_RECORD	input_record;

	assert(cb != NULL);
	assert(cb->Handle != NULL && cb->Handle != 0);

	stdin = GetStdHandle(STD_INPUT_HANDLE);
	FatalAssertion(stdin != 0, "WinConsole_ReceiverThread failed to acquire the handle of standard input.");

	while((work = WaitForSingleObject(cb->Handle, INFINITE)) != WAIT_FAILED)
	{
		assert(work == WAIT_OBJECT_0);	/* オブジェクトはシグナル状態になった */

		ReadConsoleInput(stdin, &input_record, 1, &work);

		if(input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown == TRUE)
		{
			cb->ReceiveBuffer = (char)input_record.Event.KeyEvent.uChar.AsciiChar;

			BITSET(cb->Flag, SIO_STA_INTRCV);
			HALInterruptRequest(INHNO_SERIAL);
		}
	}

	return 0;
}

static void WinConsole_CreatePort(SIOPCB * cb)
{
	BOOL result;
	result = AllocConsole();
	FatalAssertion(result != 0, "WinConsole_CreatePort failed to allocate its own console.");
	cb->Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTitle("TOPPERS/JSP SerialConsole");

		/* 受信バッファ監視用スレッドを作成 */
	CreateThread(NULL,0,WinConsole_ReceiverThread,(LPVOID)cb,0,NULL);
}

static BOOL WinConsole_PutChar(SIOPCB * cb, INT chr, BOOL rasint)
{
	BOOL  result;
	DWORD written;
	char  word;

	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_TTY));
	assert(cb->Handle != NULL && cb->Handle != 0);

		/* 標準出力に一文字送信 */
	word   = (char)chr;
	result = WriteFile(cb->Handle, &word, 1, &written, NULL);

		/* 割込み発生フラグが立っていたら、送信完了割込み要求を起こす */
	if(result != 0 && rasint == TRUE)
	{
		BITSET(cb->Flag, SIO_STA_INTSND);
		HALInterruptRequest(INHNO_SERIAL);
	}

	return result != 0 ? TRUE : FALSE;
}

static void WinConsole_ClosePort(SIOPCB * cb)
{
	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_TTY));

	if(cb->Handle != NULL && cb->Handle != 0)
	{
		FreeConsole();
		cb->Handle = NULL;
	}
}

/*===========================================================================*/

/*
 *   Windowsのコンソールを使用したシリアル入出力
 */

/* 注 : CreateConsoleScreenBufferは失敗時にINVALID_HANDLE_VALUEを返すが、
          他のシリアル入出力部と共通かするためにNULLを用いる。 */

static DWORD WINAPI ScreenBuffer_ReceiverThread(LPVOID param)
{
	SIOPCB *		cb = (SIOPCB *)param;
	DWORD			work;
	INPUT_RECORD	input_record;

	assert(cb != NULL);
	assert(cb->Handle != NULL && cb->Handle != 0);

	while((work = WaitForSingleObject(cb->Handle, INFINITE)) != WAIT_FAILED)
	{
		assert(work == WAIT_OBJECT_0);	/* オブジェクトはシグナル状態になった */

		ReadConsoleInput(cb->Handle, &input_record, 1, &work);

		if(input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown == TRUE)
		{
			cb->ReceiveBuffer = (char)input_record.Event.KeyEvent.uChar.AsciiChar;

			BITSET(cb->Flag, SIO_STA_INTRCV);
			HALInterruptRequest(INHNO_SERIAL);
		}
	}

	return 0;
}

static void ScreenBuffer_CreatePort(SIOPCB * cb)
{
	cb->Handle = CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

        /* cb->Handle should not be compared with NULL because CreateConsoleScreenBuffer returns INVALID_HANDLE_VALUE if it failed */
    FatalAssertion(cb->Handle != INVALID_HANDLE_VALUE && cb->Handle != NULL,
        "ScreenBuffer_CreatePort failed to allocate its own console.");

	SetConsoleActiveScreenBuffer(cb->Handle);
	SetConsoleTitle("TOPPERS/JSP SerialConsole");

		/* 受信バッファ監視用スレッドを作成 */
	CreateThread(NULL,0,ScreenBuffer_ReceiverThread,(LPVOID)cb,0,NULL);
}

static BOOL ScreenBuffer_PutChar(SIOPCB * cb, INT chr, BOOL rasint)
{
	BOOL  result;
	DWORD written;
	char  word;

	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_SCRBUF));
	assert(cb->Handle != NULL && cb->Handle != 0);

		/* 標準出力に一文字送信 */
	word   = (char)chr;
	result = WriteFile(cb->Handle, &word, 1, &written, NULL);

		/* 割込み発生フラグが立っていたら、送信完了割込み要求を起こす */
	if(result != 0 && rasint == TRUE)
	{
		BITSET(cb->Flag, SIO_STA_INTSND);
		HALInterruptRequest(INHNO_SERIAL);
	}

	return result != 0 ? TRUE : FALSE;
}

static void ScreenBuffer_ClosePort(SIOPCB * cb)
{
	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_SCRBUF));

	if(cb->Handle != NULL && cb->Handle != 0)
	{
		CloseHandle(cb->Handle);
		cb->Handle = NULL;
	}
}

/*===========================================================================*/

/*
 *   ほんとにシリアル出力
 */

static DWORD WINAPI RawSerial_ReceiverThread(LPVOID param)
{
	SIOPCB *	cb = (SIOPCB *)param;
	DWORD		work;
	char	    buffer;

	assert(cb != NULL);
	assert(cb->Handle != NULL && cb->Handle != 0);

	while((work = WaitForSingleObject(cb->Handle, INFINITE)) != WAIT_FAILED)
	{
		assert(work == WAIT_OBJECT_0);	/* オブジェクトはシグナル状態になった */

		ReadFile(cb->Handle, &buffer, 1, &work, NULL);

		cb->ReceiveBuffer = (char)buffer;
		BITSET(cb->Flag, SIO_STA_INTRCV);
		HALInterruptRequest(INHNO_SERIAL);
	}

	return 0;
}

static void RawSerial_CreatePort(SIOPCB * cb)
{
    cb->Handle = CreateFile("COM1", GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	FatalAssertion( cb->Handle != INVALID_HANDLE_VALUE && cb->Handle != NULL, 
                    "RawSerial_CreatePort failed to open the port 'COM1'.");

		/* 受信バッファ監視用スレッドを作成 */
	CreateThread(NULL, 0, RawSerial_ReceiverThread, (LPVOID)cb, 0, NULL);
}

static BOOL RawSerial_PutChar(SIOPCB * cb, INT chr, BOOL rasint)
{
	BOOL  result;
	DWORD written;
	char  word;

	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_RAWSIO));
	assert(cb->Handle != NULL && cb->Handle != 0);

		/* 標準出力に一文字送信 */
	word   = (char)chr;
	result = WriteFile(cb->Handle, &word, 1, &written, NULL);

		/* 割込み発生フラグが立っていたら、送信完了割込み要求を起こす */
	if(result != 0 && rasint == TRUE)
	{
		BITSET(cb->Flag, SIO_STA_INTSND);
		HALInterruptRequest(INHNO_SERIAL);
	}

	return result != 0 ? TRUE : FALSE;
}

static void RawSerial_ClosePort(SIOPCB * cb)
{
	assert(cb != NULL);
	assert(BITTEST(cb->Flag, SIO_TYP_RAWSIO));

	if(cb->Handle != NULL && cb->Handle != 0)
	{
		CloseHandle(cb->Handle);
		cb->Handle = NULL;
	}
}


/*
 *  SIOドライバの初期化ルーチン
 */
void sio_initialize(void)
{
	int i;

	for(i=0; i<TNUM_PORT; ++i)
	{
		siopcb_table[i].Flag = SIO_TYP_CONSOLE;
		siopcb_table[i].Handle = NULL;
		siopcb_table[i].ReceiveBuffer = -1;
		siopcb_table[i].versatile = NULL;
	}
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB * sio_opn_por(ID siopid, VP_INT exinf)
{
	BOOL     success;
	SIOPCB * siopcb = GET_SIOPCB(siopid);

	assert(siopcb != NULL);

	siopcb->exinf = exinf;

	success = TRUE;
	switch(SIO_TYP(siopcb->Flag))
	{
	case SIO_TYP_CONSOLE:
		HALExecuteProcedure(CreateSerialConsole,siopcb);
		break;
	case SIO_TYP_TTY:
		WinConsole_CreatePort(siopcb);
		break;
	case SIO_TYP_SCRBUF:
		HALExecuteProcedure(ScreenBuffer_CreatePort,siopcb);
		break;
	case SIO_TYP_RAWSIO:
		HALExecuteProcedure(RawSerial_CreatePort,siopcb);
		break;
	default:
		success = FALSE;
	}

	if(success == TRUE)
		BITSET(siopcb->Flag, SIO_STA_OPEN);

	return siopcb;
}

/*
 *  シリアルI/Oポートのクローズ
 */
void sio_cls_por(SIOPCB *siopcb)
{
	assert(siopcb != NULL);

	switch(SIO_TYP(siopcb->Flag))
	{
	case SIO_TYP_CONSOLE:
		HALExecuteProcedure(SerialConsole_FinalRelease,siopcb);
		break;
	case SIO_TYP_TTY:
		WinConsole_ClosePort(siopcb);
		break;
	case SIO_TYP_SCRBUF:
		HALExecuteProcedure(ScreenBuffer_ClosePort,siopcb);
		break;
	case SIO_TYP_RAWSIO:
		HALExecuteProcedure(RawSerial_ClosePort,siopcb);
		break;
	}
}

/*
 *  シリアルI/Oポートへの文字送信
 */
BOOL sio_snd_chr(SIOPCB *siopcb, INT chr)
{
	BOOL result;

	assert(siopcb != NULL);

	switch(SIO_TYP(siopcb->Flag))
	{
	case SIO_TYP_CONSOLE:
		result = SerialConsole_PushChar(siopcb, chr);
		break;
	case SIO_TYP_TTY:
		result = WinConsole_PutChar(siopcb, chr, TRUE);
		break;
	case SIO_TYP_SCRBUF:
		result = ScreenBuffer_PutChar(siopcb, chr, TRUE);
		break;
	case SIO_TYP_RAWSIO:
		result = RawSerial_PutChar(siopcb, chr, TRUE);
		break;
	}
	
	return result;
}

/*
 *  シリアルI/Oポートからの文字受信
 */
INT sio_rcv_chr(SIOPCB * siopcb)
{
	INT result;

	result = siopcb->ReceiveBuffer;
	siopcb->ReceiveBuffer = -1;

	return result;
}


/*
 *  シリアルI/Oポートからのコールバック許可
 */
void sio_ena_cbr(SIOPCB * siopcb, UINT cbrtn)
{}

/*
 *  シリアルI/Oポートからのコールバック禁止
 */
void sio_dis_cbr(SIOPCB * siopcb, UINT cbrtn)
{}


/*
 *  シリアルI/Oポート割込みハンドラ
 */
void sio_handler(void)
{
	int port;

	for(port = 0; port < TNUM_PORT; ++ port)
	{
		if(BITTEST(siopcb_table[port].Flag, SIO_STA_OPEN))
		{
				/* 受信完了割込み */
			if(BITTEST(siopcb_table[port].Flag, SIO_STA_INTRCV))
			{
					/* 受信した文字を取り出し */
				BITCLEAR(siopcb_table[port].Flag, SIO_STA_INTRCV);
				sio_ierdy_rcv(GET_SIOPCB(ID_PORT(port))->exinf);

			}

				/* 送信完了割込み */
			if(BITTEST(siopcb_table[port].Flag, SIO_STA_INTSND))
			{
				BITCLEAR(siopcb_table[port].Flag, SIO_STA_INTSND);
				sio_ierdy_snd(GET_SIOPCB(ID_PORT(port))->exinf);
			}
		}
	}
}

/*
 *  コンソールポートへの強制一文字出力 (割込みなし)
 */
void SerialRawPutc(INT chr)
{
	SIOPCB * siopcb = &siopcb_table[CONSOLE_PORTID - 1];

	assert(siopcb != NULL);

	if(!BITTEST(siopcb->Flag, SIO_STA_OPEN))
		return;

	switch(SIO_TYP(siopcb->Flag))
	{
	case SIO_TYP_CONSOLE:
		SerialConsole_PutChar(siopcb, chr, FALSE);
		break;
	case SIO_TYP_TTY:
		WinConsole_PutChar(siopcb, chr, FALSE);
		break;
	case SIO_TYP_SCRBUF:
		ScreenBuffer_PutChar(siopcb, chr, FALSE);
		break;
	case SIO_TYP_RAWSIO:
		RawSerial_PutChar(siopcb, chr, FALSE);
		break;
	}
}
