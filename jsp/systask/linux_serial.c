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
 *  @(#) $Id: linux_serial.c,v 1.11 2003/12/11 00:36:49 honda Exp $
 */

#define _LINX_SERIAL_

#include <t_services.h>
#include <s_services.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#undef __USE_MISC 
#include <unistd.h>
#include <linux_sigio.h>
#include "kernel_id.h"


/*
 *  シリアルポートの低レベル定義
 */

typedef struct hardware_serial_port_descripter {
	char   *path;		            /* UNIX 上でのファイル名 */
	int	   fd;		                /* ファイルディスクリプタ */
	struct termios	current_term;	/* 端末制御情報 */
	struct termios	saved_term;    
} HWPORT;

#define NUM_PORT	1

#define RAWPORT1	{ 0 }

/*
 *  UNIX レベルのポート初期化/シャットダウン処理
 *
 *  現在の実装では，端末を使うケースしか考えていない．本来は，open し
 *  たのが端末かどうかで処理を変えるべき．
 */

Inline void
hw_port_initialize(HWPORT *p)
{
	if (p->path) {
		p->fd = open(p->path, O_RDWR|O_NDELAY);
	}
	else {
		p->fd = 0;			/* 標準入出力を使う */
	}
	fcntl(p->fd, F_SETOWN, getpid());
	fcntl(p->fd, F_SETFL, FASYNC|FNDELAY);
    
    
    tcgetattr(p->fd, &(p->saved_term));
    
	p->current_term = p->saved_term;
    p->current_term.c_lflag &= ~(ECHO);
    p->current_term.c_lflag &= ~(ICANON);
    p->current_term.c_cc[VMIN] = 1;
    p->current_term.c_cc[VTIME] = 0;
    
    tcsetattr(p->fd, TCSAFLUSH, &(p->current_term));
}

Inline void
hw_port_terminate(HWPORT *p)
{
    tcsetattr(p->fd, TCSAFLUSH, &(p->saved_term));
	fcntl(p->fd, F_SETFL, 0);
	if (p->path) {
		close(p->fd);
	}
}


/*
 *  シリアルインタフェースドライバ用の SIGIO 通知イベントブロック
 */

static SIGIOEB	serial_sigioeb;


/*
 *  シリアルポート管理ブロックの定義
 */

typedef struct ioctl_descripter {
	int	echo;
	int	input;
	int	newline;
	int	flowc;
} IOCTL;

#define	SERIAL_BUFSZ	256	/* シリアルインタフェース用バッファのサイズ */

#define	inc(x)		(((x)+1 < SERIAL_BUFSZ) ? (x)+1 : 0)
#define	INC(x)		((x) = inc(x))


typedef struct serial_port_control_block {
	BOOL	init_flag;	/* 初期化済か？ */
	HWPORT	hwport;		/* ハードウェア依存情報 */
	ID	in_semid;	/* 受信バッファ管理用セマフォの ID */
	ID	out_semid;	/* 送信バッファ管理用セマフォの ID */

	int	in_read_ptr;	/* 受信バッファ読み出しポインタ */
	int	in_write_ptr;	/* 受信バッファ書き込みポインタ */
	int	out_read_ptr;	/* 送信バッファ読み出しポインタ */
	int	out_write_ptr;	/* 送信バッファ書き込みポインタ */
	UINT	ioctl;		/* ioctl による設定内容 */
	BOOL	send_enabled;	/* 送信をイネーブルしてあるか？ */
	BOOL	ixon_stopped;	/* STOP を受け取った状態か？ */
	BOOL	ixoff_stopped;	/* 相手に STOP を送った状態か？ */
	char	ixoff_send;	/* 相手に START/STOP を送るか？ */

	char	in_buffer[SERIAL_BUFSZ];	/* 受信バッファエリア */
	char	out_buffer[SERIAL_BUFSZ];	/* 受信バッファエリア */
} SPCB;


#define	IN_BUFFER_EMPTY(spcb) \
		((spcb)->in_read_ptr == (spcb)->in_write_ptr)
#define	IN_BUFFER_FULL(spcb) \
		((spcb)->in_read_ptr == inc((spcb)->in_write_ptr))
#define	OUT_BUFFER_FULL(spcb) \
		((spcb)->out_read_ptr == inc((spcb)->out_write_ptr))

/*
 *  モジュール内で使う関数
 */
static void	sigint_handler();
static BOOL	serial_getc(SPCB *spcb, char *c);
static BOOL	serial_putc(SPCB *spcb, char c);


/*
 *  シリアルポート管理ブロックの定義と初期化
 */

static SPCB spcb_table[NUM_PORT] = {
    {0, RAWPORT1, SEM_SERIAL1_IN, SEM_SERIAL1_OUT }
};

#define get_spcb(portid)	(&(spcb_table[(portid)-1]))
#define get_spcb_def(portid)	get_spcb((portid) ? (portid) : CONSOLE_PORTID)


/*
 *  ポートの初期化
 */
int
serial_opn_por(ID portid)
{

    SPCB	*spcb;
	ER	ercd = E_OK;;

	if (!(1 <= portid && portid <= NUM_PORT)) {
		return(E_PAR);
	}
	spcb = get_spcb(portid);

	/*
	 *  複数のタスクが同時に serial_open を呼ぶ状況には対応してい
	 *  ない．
	 */
	if (spcb->init_flag) {		/* 初期化済かのチェック */
		return(E_OK);
	}

	/*
	 *  変数の初期化
	 */
	spcb->in_read_ptr = spcb->in_write_ptr = 0;
	spcb->out_read_ptr = spcb->out_write_ptr = 0;
	spcb->ixon_stopped = spcb->ixoff_stopped = FALSE;
	spcb->ixoff_send = 0;

	/*
	 *  ハードウェア依存の初期化
	 */
	hw_port_initialize(&(spcb->hwport));
        
	/*
 	 *  プロセスを終了させるシグナルを捕まえる
         *  sigaction()で書き直した方がいいのか?
	 */
	signal(SIGHUP, sigint_handler);
	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);
        
    spcb->init_flag = TRUE;
	spcb->send_enabled = FALSE;
	return(ercd);
}

/*
 *  ポートのシャットダウン
 */

ER
serial_cls_por(ID portid)
{
	SPCB	*spcb;

	if (!(1 <= portid && portid <= NUM_PORT)) {
		return(E_PAR);		/* ポート番号のチェック */
	}

	spcb = get_spcb(portid);
	if (!(spcb->init_flag)) {	/* 初期化済かのチェック */
		return(E_OBJ);
	}

	/*
	 *  ハードウェア依存のシャットダウン処理
	 */
	syscall(loc_cpu());
	hw_port_terminate(&(spcb->hwport));
	syscall(unl_cpu());

	spcb->init_flag = FALSE;
	return(E_OK);
}

/*
 *  プロセスを終了させるシグナルに対するハンドラ
 */
void
sigint_handler()
{
	SPCB	*spcb;
	int	i;

	for (i = 1; i <= NUM_PORT; i++) {
		spcb = get_spcb(i);
		if (spcb->init_flag) {
			hw_port_terminate(&(spcb->hwport));
		}
	}
	exit(0);
}

/*
 *  フローコントロール関係の定義
 */
#define	STOP	'\023'		/* Control-S */
#define	START	'\021'		/* Control-Q */

#define	IXOFF_STOP	64	/* buffer area size to send STOP */
#define	IXOFF_START	128	/* buffer area size to send START */

#define	in_buf_area(p)							\
		((spcb->in_read_ptr >= spcb->in_write_ptr) ?		\
		 (spcb->in_read_ptr - spcb->in_write_ptr) :		\
		 (spcb->in_read_ptr + SERIAL_BUFSZ - spcb->in_write_ptr))
/*
 *  ユーティリティルーチン
 */

Inline BOOL
read_char(SPCB *spcb, char *c)
{
	int	n;

	if ((n = read(spcb->hwport.fd, c, 1)) == 1) {
		return(1);
	}
	assert(n < 0 && errno == EWOULDBLOCK);
	return(0);
}

Inline BOOL
write_char(SPCB *spcb, char c)
{
	int	n;

	if ((n = write(spcb->hwport.fd, &c, 1)) == 1) {
		return(1);
	}
	assert(n < 0 && errno == EWOULDBLOCK);
	return(0);
}

/*
 *  シリアルポートからの受信
 */

static BOOL
serial_getc(SPCB *spcb, char *c)
{
	BOOL	buffer_empty;

	syscall(loc_cpu());
	*c = spcb->in_buffer[spcb->in_read_ptr];
        if (inc(spcb->in_write_ptr) == spcb->in_read_ptr) {
            /*
             *  バッファフル状態が解除されたら、割り込みが入ったのと
             *  同じ振る舞いをさせる。
             */
            kill(getpid(), SIGIO);
        }
        
	INC(spcb->in_read_ptr);

	if (spcb->ixoff_stopped && (in_buf_area(spcb) > IXOFF_START)) {
		if (!write_char(spcb, START)) {
			spcb->ixoff_send = START;
		}
		spcb->ixoff_stopped = FALSE;
	}
	buffer_empty = IN_BUFFER_EMPTY(spcb);
	syscall(unl_cpu());
	return(buffer_empty);
}

ER_UINT
serial_rea_dat(ID portid, char *buf, UINT len)
{
    	SPCB	*spcb;
	BOOL	buffer_empty;
	char	c;
	int	i;


	if (sns_dpn()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(0 <= portid && portid <= NUM_PORT)) {
		return(E_PAR);		/* ポート番号のチェック */
	}

	spcb = get_spcb_def(portid);
	if (!(spcb->init_flag)) {	/* 初期化済かのチェック */
		return(E_OBJ);
	}
	if (len == 0) {
		return(len);
	}


	syscall(wai_sem(spcb->in_semid));
	buffer_empty = FALSE;
	for (i = 0; i < len; i++) {
		buffer_empty = serial_getc(spcb, &c);
        if ((spcb->ioctl & IOCTL_ECHO) != 0) {
			syscall(wai_sem(spcb->out_semid));
			if (!serial_putc(spcb, c)) {
				syscall(sig_sem(spcb->out_semid));
			}
		}
		*buf++ = c;
		if (buffer_empty && i < len - 1) {
			syscall(wai_sem(spcb->in_semid));
		}
	}
	if (!buffer_empty) {
		syscall(sig_sem(spcb->in_semid));
	}
	return(len);
}


/*
 * シリアルポートへの送信
 */

static BOOL
serial_putc(SPCB *spcb, char c)
{
	BOOL	buffer_full;

    if (c == '\n' && (spcb->ioctl & IOCTL_CRLF) != 0) {
		if (serial_putc(spcb, '\r')) {
			syscall(wai_sem(spcb->out_semid));
		}
	}

	syscall(loc_cpu());
	if (!(spcb->ixon_stopped) && write_char(spcb, c)) {
		buffer_full = FALSE;
	}
	else {
		spcb->out_buffer[spcb->out_write_ptr] = c;
		INC(spcb->out_write_ptr);
		buffer_full = OUT_BUFFER_FULL(spcb);
	}
	syscall(unl_cpu());
	return(buffer_full);
}

ER_UINT
serial_wri_dat(ID portid, char *buf, UINT len)
{
	SPCB	*spcb;
	BOOL	buffer_full;
	int	i;

	if (sns_dpn()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(0 <= portid && portid <= NUM_PORT)) {
		return(E_PAR);		/* ポート番号のチェック */
	}

	spcb = get_spcb_def(portid);
	if (!(spcb->init_flag)) {	/* 初期化済かのチェック */
		return(E_OBJ);
	}

	syscall(wai_sem(spcb->out_semid));
	buffer_full = FALSE;
	for (i = 0; i < len; i++) {
		buffer_full = serial_putc(spcb, *buf++);
		if (buffer_full && i < len - 1) {
			syscall(wai_sem(spcb->out_semid));
		}
	}
	if (!buffer_full) {
		syscall(sig_sem(spcb->out_semid));
	}
	return(len);
}

/*
 *  シリアルポートの制御
 */

int
serial_ctl_por(ID portid, UINT ioctl)
{
	SPCB	*spcb;

	if (sns_ctx()) {		/* コンテキストのチェック */
		return(E_CTX);
	}
	if (!(0 <= portid && portid <= NUM_PORT)) {
		return(E_PAR);		/* ポート番号のチェック */
	}

	spcb = get_spcb_def(portid);
	if (!(spcb->init_flag)) {	/* 初期化済かのチェック */
		return(E_OBJ);
	}

    spcb->ioctl = ioctl;
    return(E_OK);
}


/*
 *  シリアルポート割込みハンドラ
 */

static BOOL
serial_int_handler(ID portid)
{
	SPCB	*spcb;
	BOOL	flag;
	char	c;

	spcb = get_spcb(portid);
	flag = 0;

	/*
	 *  1文字受信処理
	 *
	 *  まず，バッファフルでない場合に，1文字読んでみる．読めれば，
	 *  それに応じた処理を行う．
	 */
	if (inc(spcb->in_write_ptr) != spcb->in_read_ptr
            && read_char(spcb, &c)) {
		if ((spcb->ioctl & IOCTL_FCSND) != 0 && c == STOP) {
			spcb->ixon_stopped = TRUE;
		}
        else if (((spcb->ioctl & IOCTL_FCSND) != 0 || spcb->ixon_stopped)
                                 && (c == START || (spcb->ioctl & IOCTL_FCANY) != 0)) {
			spcb->ixon_stopped = FALSE;
		}
		else {
			spcb->in_buffer[spcb->in_write_ptr] = c;
                        if(spcb->in_read_ptr == spcb->in_write_ptr){
                            syscall(sig_sem(spcb->in_semid));
                        }
                        
			INC(spcb->in_write_ptr);
                        
			if ((spcb->ioctl & IOCTL_FCRCV) != 0 && !(spcb->ixoff_stopped)
					&& (in_buf_area(p) < IXOFF_STOP)) {
				spcb->ixoff_stopped = TRUE;
				spcb->ixoff_send = STOP;
			}
		}
		flag = 1;
	}

	/*
	 *  1文字送信処理
	 */
	if (spcb->ixoff_send) {
		if (write_char(spcb, spcb->ixoff_send)) {
			spcb->ixoff_send = 0;
			flag = 1;
		}
	}
	else if (!(spcb->ixon_stopped)
                      && spcb->out_read_ptr != spcb->out_write_ptr) {
		if (write_char(spcb, spcb->out_buffer[spcb->out_read_ptr])) {
                    if(OUT_BUFFER_FULL(spcb)){
			syscall(sig_sem(spcb->out_semid));
                    }
                    INC(spcb->out_read_ptr);                        
                    flag = 1;
		}
	}
	return(flag);
}



/*
 *  SIGIO コールバックルーチン
 */

static BOOL
serial_sigio_callback(VP arg)
{
	BOOL	flag;

	do {
//		syscall(loc_cpu());
		flag = serial_int_handler(1);
//		syscall(unl_cpu());
	} while (flag);
	return(0);
}



/*
 *  シリアルインタフェースドライバの起動
 */

void
serial_initialize(VP_INT exinf)
{
	serial_sigioeb.callback = serial_sigio_callback;
	serial_sigioeb.arg = (VP) 0;
	syscall(enqueue_sigioeb_initialize(&serial_sigioeb));
}
