/***************************************************************************
                          ib.h  -  header file for gpib library
                             -------------------

    copyright            : (C) 2002 by Frank Mori Hess
    email                : fmhess@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PUBLIC_GPIB_H
#define _PUBLIC_GPIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "gpib_user.h"

typedef uint16_t Addr4882_t;
static const Addr4882_t NOADDR = (Addr4882_t)-1;

/* tells RcvRespMsg() to stop on EOI */
static const int STOPend = 0x100;

enum sad_special_address
{
	NO_SAD = 0,
	ALL_SAD = -1
};

enum send_eotmode
{
	NULLend = 0,
	DABend = 1,
	NLend = 2
};

extern volatile int ibsta, ibcnt, iberr;
extern volatile long ibcntl;

extern void AllSPoll( int board_desc, const Addr4882_t addressList[], short resultList[] );
extern void AllSpoll( int board_desc, const Addr4882_t addressList[], short resultList[] );
extern void DevClear( int board_desc, Addr4882_t address );
extern void DevClearList( int board_desc, const Addr4882_t addressList[] );
extern void EnableLocal( int board_desc, const Addr4882_t addressList[] );
extern void EnableRemote( int board_desc, const Addr4882_t addressList[] );
extern void FindLstn( int board_desc, const Addr4882_t padList[],
	Addr4882_t resultList[], int maxNumResults );
extern void FindRQS( int board_desc, const Addr4882_t addressList[], short *result );
extern void PassControl( int board_desc, Addr4882_t address );
extern void PPoll( int board_desc, short *result );
extern void PPollConfig( int board_desc, Addr4882_t address, int dataLine, int lineSense );
extern void PPollUnconfig( int board_desc, const Addr4882_t addressList[] );
extern void RcvRespMsg( int board_desc, void *buffer, long count, int termination );
extern void ReadStatusByte( int board_desc, Addr4882_t address, short *result );
extern void Receive( int board_desc, Addr4882_t address,
	void *buffer, long count, int termination );
extern void ReceiveSetup( int board_desc, Addr4882_t address );
extern void ResetSys( int board_desc, const Addr4882_t addressList[] );
extern void Send( int board_desc, Addr4882_t address, const void *buffer,
	long count, int eot_mode );
extern void SendCmds( int board_desc, const void *cmds, long count );
extern void SendDataBytes( int board_desc, const void *buffer,
	long count, int eotmode );
extern void SendIFC( int board_desc );
extern void SendLLO( int board_desc );
extern void SendList( int board_desc, const Addr4882_t addressList[], const void *buffer,
	long count, int eotmode );
extern void SendSetup( int board_desc, const Addr4882_t addressList[] );
extern void SetRWLS( int board_desc, const Addr4882_t addressList[] );
extern void TestSRQ( int board_desc, short *result );
extern void TestSys( int board_desc, const Addr4882_t addressList[],
	short resultList[] );
extern int ThreadIbsta( void );
extern int ThreadIberr( void );
extern int ThreadIbcnt( void );
extern long ThreadIbcntl( void );
extern void Trigger( int board_desc, Addr4882_t address );
extern void TriggerList( int board_desc, const Addr4882_t addressList[] );
extern void WaitSRQ( int board_desc, short *result );
extern int ibask( int ud, int option, int *value );
extern int ibbna( int ud, char *board_name );
extern int ibcac( int ud, int synchronous );
extern int ibclr( int ud );
extern int ibcmd( int ud, const void *cmd, long cnt );
extern int ibcmda( int ud, const void *cmd, long cnt );
extern int ibconfig( int ud, int option, int value );
extern int ibdev( int board_index, int pad, int sad, int timo, int send_eoi, int eosmode );
extern int ibdma( int ud, int v );
extern int ibeot( int ud, int v );
extern int ibeos( int ud, int v );
extern int ibevent( int ud, short *event );
extern int ibfind( const char *dev );
extern int ibgts(int ud, int shadow_handshake);
extern int ibist( int ud, int ist );
extern int iblines( int ud, short *line_status );
extern int ibln( int ud, int pad, int sad, short *found_listener );
extern int ibloc( int ud );
extern int ibonl( int ud, int onl );
extern int ibpad( int ud, int v );
extern int ibpct( int ud );
extern int ibppc( int ud, int v );
extern int ibrd( int ud, void *buf, long count );
extern int ibrda( int ud, void *buf, long count );
extern int ibrdf( int ud, const char *file_path );
extern int ibrpp( int ud, char *ppr );
extern int ibrsc( int ud, int v );
extern int ibrsp( int ud, char *spr );
extern int ibrsv( int ud, int v );
extern int ibsad( int ud, int v );
extern int ibsic( int ud );
extern int ibsre( int ud, int v );
extern int ibstop( int ud );
extern int ibtmo( int ud, int v );
extern int ibtrg( int ud );
extern int ibwait( int ud, int mask );
extern int ibwrt( int ud, const void *buf, long count );
extern int ibwrta( int ud, const void *buf, long count );
extern int ibwrtf( int ud, const char *file_path );
extern const char* gpib_error_string( int iberr );

static __inline__ Addr4882_t MakeAddr( unsigned int pad, unsigned int sad )
{
	Addr4882_t address;

	address = ( pad & 0xff );
	address |= ( sad << 8 ) & 0xff00;
	return address;
}

static __inline__ unsigned int GetPAD( Addr4882_t address )
{
	return address & 0xff;
}

static __inline__ unsigned int GetSAD( Addr4882_t address )
{
	return ( address >> 8 ) & 0xff;
}

#ifdef __cplusplus
}
#endif

#endif	/* _PUBLIC_GPIB_H */
