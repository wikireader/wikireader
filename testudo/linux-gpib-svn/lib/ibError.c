/***************************************************************************
                          lib/ibError.c
                             -------------------

    copyright            : (C) 2001,2002,2003 by Frank Mori Hess
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

#include "ib_internal.h"

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char *routptr[]= {
    "ibrd",
    "ibwrt",
    "ibcmd",
    "ibwait",
    "ibrpp",
    "ibonl",
    "ibsic",
    "ibsre",
    "ibgts",
    "ibcac",
    "ibpoke",
    "iblines",
    "ibpad",
    "ibsad",
    "ibtmo",
    "ibeot",
    "ibeos",
    "ibrsv",  /*17*/

    "dvtrg",
    "dvclr",
    "dvrsp",
    "dvrd",
    "dvwrt", /*22*/

    "cfcbase",
    "cfcirq",
    "cfcdma", /*25*/

    "unknown"
  };

char *ibVerbCode(int code)
{

  if( code >=0 && code <=17 )    return routptr[code];
  else if( code >=100 && code <=104 ) return routptr[18+(code-100)];
  else if( code >=200 && code <=202 ) return routptr[23+(code-200)];
  else return routptr[26];
}

void ibPutMsg (char *format,...) 
{

va_list ap;

  time_t tm;
  char strtime[60];


    va_start(ap,format);
    time(&tm);
    strftime(strtime,59,"%c",gmtime(&tm));

    fprintf(stderr,"\n gpib-message   :[%s] ",strtime);
    vfprintf(stderr, format, ap);
    va_end(ap);

}

void ibPutErrlog(int ud,char *routine)
{

time_t tm;
char strtime[60];

    if( ibsta & ERR ) {
    time(&tm);
    strftime(strtime,59,"%c",gmtime(&tm));

    if(ud>=0) fprintf(stderr, "\n %-15s:[%s](%s)< ",routine,strtime, ibConfigs[ud]->name);
    else      fprintf(stderr, "\n %-15s:[%s](-)< " ,routine,strtime);

    if ( ibsta & ERR )  fprintf(stderr," ERR");
    if ( ibsta & TIMO ) fprintf(stderr," TIMO");
    if ( ibsta & END )  fprintf(stderr," END");
    if ( ibsta & SRQI ) fprintf(stderr," SRQI");
    /*if ( ibsta & RQS )  fprintf(stderr," RQS");*/
    if ( ibsta & CMPL ) fprintf(stderr," CMPL");
    /*if ( ibsta & LOK )  fprintf(stderr," LOK");*/
    /*if ( ibsta & REM )  fprintf(stderr," REM");*/
    if ( ibsta & CIC )  fprintf(stderr," CIC");
    if ( ibsta & ATN )  fprintf(stderr," ATM");
    if ( ibsta & TACS ) fprintf(stderr," TACS");
    if ( ibsta & LACS ) fprintf(stderr," LACS");
    /*if ( ibsta & DTAS ) fprintf(stderr," DATS");*/
    /*if ( ibsta & DCAS ) fprintf(stderr," DCTS");*/

    fprintf(stderr,"> ");

    if ( iberr == EDVR) fprintf(stderr," EDVR <OS Error>\n");
    if ( iberr == ECIC) fprintf(stderr," ECIC <Not CIC>\n");
    if ( iberr == ENOL) fprintf(stderr," ENOL <No Listener>\n");
    if ( iberr == EADR) fprintf(stderr," EADR <Adress Error>\n");
    if ( iberr == EARG) fprintf(stderr," ECIC <Invalid Argument>\n");
    if ( iberr == ESAC) fprintf(stderr," ESAC <No Sys Ctrlr>\n");
    if ( iberr == EABO) fprintf(stderr," EABO <Operation Aborted>\n");
    if ( iberr == ENEB) fprintf(stderr," ENEB <No Gpib Board>\n");
    if ( iberr == EOIP) fprintf(stderr," EOIP <Async I/O in prg>\n");
    if ( iberr == ECAP) fprintf(stderr," ECAP <No Capability>\n");
    if ( iberr == EFSO) fprintf(stderr," EFSO <File sys. error>\n");
    if ( iberr == EBUS) fprintf(stderr," EBUS <Command error>\n");
    if ( iberr == ESTB) fprintf(stderr," ESTB <Status byte lost>\n");
    if ( iberr == ESRQ) fprintf(stderr," ESRQ <SRQ stuck on>\n");
    if ( iberr == ETAB) fprintf(stderr," ETAB <Table Overflow>\n");
    if ( iberr == ETAB) fprintf(stderr," ETAB <Device Table Overflow>\n");

    if ( iberr == EDVR && ibcnt != 0) {
      fprintf(stderr,"               -- errno=%d (%s)\n",ibcnt,strerror(ibcnt));
    }
  }
}






