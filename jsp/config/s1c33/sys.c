/*
 *  TOPPERS/JSP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 *
 *  Additional routine
 *
 *  Copyright (C) 2003 by SEIKO EPSON Corp, JAPAN
 *
 *  上記著作権者は，Free Software Foundation によって公表されている
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 *
 */

#include "cpu_rename.h"

unsigned char WRITE_BUF[65];

/* buffer for simulated stdin, WRITE_BUF[0] is size (1-0x40, 0 means no data)
   WRITE_BUF[1-64] is buffer area for data, max 64 bytes
   used in write () */

unsigned char READ_BUF[65];

/* buffer for simulated stdin, READ_BUF[0] is size (1-0x40, 0 means EOF)
   READ_BUF[1-64] is buffer area for data, max 64 bytes
   used in read() */

static unsigned char READ_EOF; /* if 1: READ_BUFFER become EOF, 0: not EOF */

/*
 *  void _exit
 *	_exit execute inifinity loop.
 */

void _exit()
{
LOOP:
	goto LOOP;
}

/*
 *  void _init_sys
 *	_init_sys initialize read() and write() bffer area
 */

void init_sys()
{
	READ_EOF = 0;		/* not EOF */
	READ_BUF[0] = 0;	/* BUFFER is empty */
}

/*
 *  read
 *	Read() get and return required bytes with using simulated input.
 *	If EOF return 0.
 *	READ_FLASH: is break point label for stdin
 *	READ_BUF is buffer area for stdin
 */

int read (int fhDummy, char *psReadBuf, int iReadBytes)
{
	int iBytes;		/* data size written to psReadBuf */
	int iSize;		/* data size in READ_BUF */
	char *psBuf;		/* top of read buffer */
	static int iNdxPos;	/* current positon in READ_BUF*/

/* start */

	iBytes = 0;		/* no read now */
	psBuf = psReadBuf;

/* This loop repeat for each byte to copy READ_BUF to psReadBuf */

	for (;;)
	    {

/* if iReadByte become 0, return */

		if (iReadBytes == 0)	/* if required size become 0, return */
			return(iBytes);

/* if EOF, return 0 */

		if (READ_EOF == 1)
			return(iBytes);

/* if there is data, copy 1 byte */

		iSize = READ_BUF[0];
		if (iSize > 0 )
		    {
		    	*psBuf = READ_BUF[iNdxPos];
		    	psBuf++;
		    	iReadBytes--;
		    	iNdxPos++;
		    	iSize--;
			iBytes++;
		    	READ_BUF[0] = (unsigned char)(iSize & 0xff);
		    }

/* if no data, read 0-64 bytes from simulated input */

		else
		    {
			asm(".global READ_FLASH");
			asm("READ_FLASH:");	/* label for simulated stdin */
			if (READ_BUF[0] == 0)
				READ_EOF = 1;	/* if size is 0, EOF */
		    	iNdxPos = 1;	/* reset index position */
		    }
	    } /* back to for (;;) */
}

/*
 *  write
 *	write datas with using simulated stdout
 *	WRITE_FLASH: is break point label for stdout
 *      WRITE_BUF is buffer area for stdout
 */

int write (int fhDummy, char *psWriBuf, int iWriBytes)
{
	int iBytes;	/* remain data bytes waiting WRITE_BUF */
	int iSize;	/* data size to write to WRITE_BUF */
	int iCount;	/* counter to copy data to WRITE_BUF */

	iBytes = iWriBytes;

	for (;;){				/* repeat each 255 bytes */

/* if remain 0, return original size */

	if (iBytes == 0)		/* remain size become 0, so return */
		return(iWriBytes);

/* if remain > 64, write size is 64 */

	if (iBytes > 64)
	{				/* over 64 */
		iSize = 64;				/* 64 bytes to WRITE_BUF */
		iBytes = iBytes - 64;	/* remain data */
	}
	else
	{						/* not over 64 */
		iSize = iBytes;		/* under 64 bytes to WRITE_BUF */
		iBytes = 0;			/* no remain data */
	}

/* copy psWriBuf to WRITE_BUF */

	WRITE_BUF[0] = (unsigned char)(iSize & 0xff);	/* set size */
	for (iCount = 1 ; iCount <= iSize ; iCount++)
	{
		WRITE_BUF[iCount] = *psWriBuf;		/* copy data */
		psWriBuf++;
	}
	asm(".global WRITE_FLASH");
	asm("WRITE_FLASH:");		/* label for simulated stdout */

    }	/* back to for (;;) */

	return(iSize);
}

