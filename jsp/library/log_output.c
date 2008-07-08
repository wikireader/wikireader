/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2005 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
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
 *  @(#) $Id: log_output.c,v 1.11 2007/03/27 08:56:12 hiro Exp $
 */

/*
 *	システムログのフォーマット出力
 */

#include <t_services.h>

/*
 *  ポインタを格納できる整数型のデフォルトの定義
 */
#ifndef _intptr_
#define _intptr_	long
#endif /* _intptr_ */

/*
 *  数値を文字列に変換
 */
#define CONVERT_BUFLEN	((sizeof(_intptr_) * CHAR_BIT + 2) / 3)
					/* _intptr_型の数値の最大文字数 */
static void
convert(unsigned _intptr_ val, unsigned int radix, const char *radchar,
		int width, int minus, int padzero, void (*putc)(char))
{
	char	buf[CONVERT_BUFLEN];
	int	i, j;

	i = 0;
	do {
		buf[i++] = radchar[val % radix];
		val /= radix;
	} while (val != 0);

	width -= minus;
	if (minus > 0 && padzero > 0) {
		(*putc)('-');
	}
	for (j = i; j < width; j++) {
		(*putc)((char)(padzero > 0 ? '0' : ' '));
	}
	if (minus > 0 && padzero <= 0) {
		(*putc)('-');
	}
	while (i > 0) {
		(*putc)(buf[--i]);
	}
}

/*
 *  ログ情報のフォーマット出力
 */
static const char raddec[] = "0123456789";
static const char radhex[] = "0123456789abcdef";
static const char radHEX[] = "0123456789ABCDEF";

void
syslog_printf(const char *format, VP_INT *args, void (*putc)(char))
{
	int		c;
	int		width;
	int		padzero;
	_intptr_	val;
	const char	*str;

	while ((c = *format++) != '\0') {
		if (c != '%') {
			(*putc)((char) c);
			continue;
		}

		width = padzero = 0;
		if ((c = *format++) == '0') {
			padzero = 1;
			c = *format++;
		}
		while ('0' <= c && c <= '9') {
			width = width * 10 + c - '0';
			c = *format++;
		}
		if (c == 'l') {
			c = *format++;
		}
		switch (c) {
		case 'd':
			val = (_intptr_)(*args++);
			if (val >= 0) {
				convert((unsigned _intptr_) val, 10, raddec,
						width, 0, padzero, putc);
			}
			else {
				convert((unsigned _intptr_)(-val), 10, raddec,
						width, 1, padzero, putc);
			}
			break;
		case 'u':
			val = (_intptr_)(*args++);
			convert((unsigned _intptr_) val, 10, raddec,
						width, 0, padzero, putc);
			break;
		case 'x':
		case 'p':
			val = (_intptr_)(*args++);
			convert((unsigned _intptr_) val, 16, radhex,
						width, 0, padzero, putc);
			break;
		case 'X':
			val = (_intptr_)(*args++);
			convert((unsigned _intptr_) val, 16, radHEX,
						width, 0, padzero, putc);
			break;
		case 'c':
			(*putc)((char)(_intptr_)(*args++));
			break;
		case 's':
			str = (const char *)(*args++);
			while ((c = *str++) != '\0') {
				(*putc)((char) c);
			}
			break;
		case '%':
			(*putc)('%');
			break;
		case '\0':
			format--;
			break;
		default:
			break;
		}
	}
	(*putc)('\n');
}

void
syslog_print(SYSLOG *p_log, void (*putc)(char))
{
	switch (p_log->logtype) {
	case LOG_TYPE_COMMENT:
		syslog_printf((const char *)(p_log->loginfo[0]),
					&(p_log->loginfo[1]), putc);
		break;
	case LOG_TYPE_ASSERT:
		syslog_printf("%s:%u: Assertion `%s' failed.",
					&(p_log->loginfo[0]), putc);
		break;
	}
}

static void
syslog_lostmsg(INT lost, void (*putc)(char))
{
	VP_INT	lostinfo[1];

	lostinfo[0] = (VP_INT) lost;
	syslog_printf("%d messages are lost.", lostinfo, putc);
}

void
syslog_output(void (*putc)(char))
{
	SYSLOG	log;
	INT	lostnum, n;

	lostnum = 0;
	while ((n = vrea_log(&log)) >= 0) {
		lostnum += n;
		if (log.logtype < LOG_TYPE_COMMENT) {
			continue;
		}
		if (lostnum > 0) {
			syslog_lostmsg(lostnum, putc);
			lostnum = 0;
		}
		syslog_print(&log, putc);
	}
	if (lostnum > 0) {
		syslog_lostmsg(lostnum, putc);
	}
}
