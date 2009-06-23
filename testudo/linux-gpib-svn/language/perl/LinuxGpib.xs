#include <gpib/ib.h>
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <stdlib.h>

static int
not_here(char *s)
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(char *name, int len, int arg)
{
    errno = EINVAL;
    return 0;
}

MODULE = LinuxGpib PACKAGE = LinuxGpib
PROTOTYPES: ENABLE

double
constant(sv,arg)
PREINIT:
	STRLEN len;
INPUT:
	SV *sv
	char *s = SvPV(sv, len);
	int arg
CODE:
	RETVAL = constant(s,len,arg);
OUTPUT:
	RETVAL


int
ibcac(ud, v)
	int	ud
	int	v

int
ibclr(ud)
	int	ud

int
ibcmd(ud, cmd, cnt)
	int	ud
	char *	cmd
	unsigned long	cnt

int
ibconfig(ud, option, value)
	int	ud
	int	option
	int	value

int
ibdev(minor, pad, sad, timo, eot, eos)
	int	minor
	int	pad
	int	sad
	int	timo
	int	eot
	int	eos

int
ibdma(ud, v)
	int	ud
	int	v

int
ibeot(ud, v)
	int	ud
	int	v

int
ibevent(int ud, OUT short event)

int
ibfind(dev)
	char *	dev

int
ibgts(ud, v)
	int	ud
	int	v

int
iblines(int ud, OUT short line_status)

int
ibloc(ud)
	int	ud

int
ibonl(ud, onl)
	int	ud
	int	onl

int
ibpad(ud, v)
	int	ud
	int	v

int
ibrd(ud, rd, cnt)
	int	ud
	SV  *rd
	unsigned long	cnt
PREINIT:
	int i;
	char *buf;
CODE:
	buf = malloc( cnt + 1 );
	if( buf == NULL )
		croak( "malloc() returned NULL in ibrd()\n" );

	for(i = 0; i <= cnt; i++) { buf[i] = 0; }
	RETVAL = ibrd(ud, buf, cnt);
	sv_setpvn(rd, buf, cnt + 1);
	free( buf );
OUTPUT:
	RETVAL

int
ibrdi(ud, array, cnt)
	int	ud
	AV  *array
	unsigned long	cnt
PREINIT:
	int i;
	char *buf;
CODE:
	av_clear( array );
	buf = malloc( cnt );
	if( buf == NULL )
		croak( "malloc() returned NULL in ibrdi()\n" );
	RETVAL = ibrd(ud, buf, cnt);
	if( ( RETVAL & ERR ) == 0 )
	{
		for( i = 0; i < ThreadIbcntl(); i++ )
		{
			av_push( array, newSViv( buf[ i ] & 0xff ) );
		}
	}
	free( buf );
OUTPUT:
	RETVAL

int
ibrpp(ud, ppr)
	int	ud
	SV *ppr
CODE:
	char response;

	RETVAL = ibrsp( ud, &response );
	if( ( RETVAL & ERR ) == 0 )
		sv_setiv( ppr, response );
OUTPUT:
	RETVAL

int
ibrsp(ud, result)
	int	ud
	SV *result
CODE:
	char status;

	RETVAL = ibrsp( ud, &status );
	if( ( RETVAL & ERR ) == 0 )
		sv_setiv( result, status );
OUTPUT:
	RETVAL

int
ibrsv(ud, v)
	int	ud
	int	v

int
ibsad(ud, v)
	int	ud
	int	v

int
ibsic(ud)
	int	ud

int
ibsre(ud, v)
	int	ud
	int	v

int
ibtmo(ud, v)
	int	ud
	int	v

int
ibtrg(ud)
	int	ud

int
ibwait(ud, mask)
	int	ud
	int	mask

int
ibwrt(ud, rd, cnt)
	int	ud
	char *	rd
	unsigned long	cnt

int
ibwrti(ud, array, cnt)
	int	ud
	AV  *array
	unsigned long	cnt
PREINIT:
	int i;
	char *buf;
	SV **sv_ptr;
CODE:
	buf = malloc( cnt );
	if( buf == NULL )
		croak( "malloc() returned NULL in ibwrti()\n" );
	for( i = 0; i < cnt; i++ )
	{
		sv_ptr = av_fetch( array, i, 0 );
		if( sv_ptr == NULL )
			croak( "av_fetch returned NULL in ibwrti()\nn" );
		buf[ i ] = SvIV(*sv_ptr);
	}
	RETVAL = ibwrt(ud, buf, cnt);
	free( buf );
OUTPUT:
	RETVAL

int
ThreadIbsta()

int
ThreadIberr()

int
ThreadIbcnt()

long
ThreadIbcntl()
