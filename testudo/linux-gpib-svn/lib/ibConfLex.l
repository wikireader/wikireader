%option noyywrap
%option nounput
%option yylineno
%option outfile="ibConfLex.c" header-file="ibConfLex.h"
%option prefix="gpib_yy"
%option reentrant

%{

#include "ibConfYacc.h"
#include "ib_internal.h"
#include <string.h>

%}

%x COMMENT
%x STRING

%%

%{
%}

<COMMENT,STRING,INITIAL>\n

"/*" { BEGIN(COMMENT); }
<COMMENT>[^*\n]*
<COMMENT>"*"+[^*/i\n]*
<COMMENT>"*"+"/" BEGIN(INITIAL);

\" { BEGIN(STRING); }
<STRING>[^\"]*\" {
	if(strlen(gpib_yyget_text(yyscanner)) > 0)
		gpib_yyget_text(yyscanner)[strlen(gpib_yyget_text(yyscanner)) - 1] = 0;
	gpib_lvalp->sval = gpib_yyget_text(yyscanner);
	BEGIN(INITIAL);
	return ( T_STRING );
}

interface 	     { return (T_INTERFACE);}

base         { return (T_BASE);}
irq          { return (T_IRQ);}
dma          { return (T_DMA);}
pci_bus      { return (T_PCI_BUS);}
pci_slot      { return (T_PCI_SLOT);}

device	     { return(T_DEVICE);}

name         { return (T_NAME);}
board_type         { return (T_BOARD_TYPE);}
minor         { return (T_MINOR);}

pad          { return(T_PAD);}
sad          { return(T_SAD);}
eos          { return(T_EOSBYTE);}
timeout |
timo	     { return(T_TIMO);}

set-reos	{ return T_REOS;}
set-bin	{ return T_BIN; }
set-xeos	{ return T_XEOS; }
set-eot	{ return T_EOT; }

init-string	{ printf( "init-string deprecated\n" ); return T_INIT_S; }
init-flags	{ printf( "init-flags deprecated\n" ); return T_INIT_F; }
set-clear	{ printf( "set-clear deprecated\n" ); return T_DCL;  }

LLO           { return T_LLO; }
DCL           { return T_DCL; }
EXCL          { return T_EXCL;}

controller    |
master        { return T_MASTER; }

autopoll      { return T_AUTOPOLL; }

yes         { gpib_lvalp->bval=1; return(T_BOOL);}
no          { gpib_lvalp->bval=0; return(T_BOOL);}


none         { gpib_lvalp->ival=0 ; return(T_TIVAL);}
10us         { gpib_lvalp->ival=10; return(T_TIVAL);}
30us         { gpib_lvalp->ival=30; return(T_TIVAL);}
100us        { gpib_lvalp->ival=100; return(T_TIVAL);}
300us        { gpib_lvalp->ival=300; return(T_TIVAL);}
1ms          { gpib_lvalp->ival=1000; return(T_TIVAL);}
3ms          { gpib_lvalp->ival=3000; return(T_TIVAL);}
10ms         { gpib_lvalp->ival=10000; return(T_TIVAL);}
30ms         { gpib_lvalp->ival=30000; return(T_TIVAL);}
100ms        { gpib_lvalp->ival=100000; return(T_TIVAL);}
300ms        { gpib_lvalp->ival=300000; return(T_TIVAL);}
1s           { gpib_lvalp->ival=1000000; return(T_TIVAL);}
3s           { gpib_lvalp->ival=3000000; return(T_TIVAL);}
10s          { gpib_lvalp->ival=1000000; return(T_TIVAL);}
30s          { gpib_lvalp->ival=30000000; return(T_TIVAL);}
100s         { gpib_lvalp->ival=100000000; return(T_TIVAL);}
300s         { gpib_lvalp->ival=300000000; return(T_TIVAL);}
1000s        { gpib_lvalp->ival=1000000000; return(T_TIVAL);}

TNONE         { gpib_lvalp->ival=TNONE ; return(T_NUMBER);}
T10us         { gpib_lvalp->ival=T10us; return(T_NUMBER);}
T30us         { gpib_lvalp->ival=T30us; return(T_NUMBER);}
T100us        { gpib_lvalp->ival=T100us; return(T_NUMBER);}
T300us        { gpib_lvalp->ival=T300us; return(T_NUMBER);}
T1ms          { gpib_lvalp->ival=T1ms; return(T_NUMBER);}
T3ms          { gpib_lvalp->ival=T3ms; return(T_NUMBER);}
T10ms         { gpib_lvalp->ival=T10ms; return(T_NUMBER);}
T30ms         { gpib_lvalp->ival=T30ms; return(T_NUMBER);}
T100ms        { gpib_lvalp->ival=T100ms; return(T_NUMBER);}
T300ms        { gpib_lvalp->ival=T300ms; return(T_NUMBER);}
T1s           { gpib_lvalp->ival=T1s; return(T_NUMBER);}
T3s           { gpib_lvalp->ival=T3s; return(T_NUMBER);}
T10s          { gpib_lvalp->ival=T10s; return(T_NUMBER);}
T30s          { gpib_lvalp->ival=T30s; return(T_NUMBER);}
T100s         { gpib_lvalp->ival=T100s; return(T_NUMBER);}
T300s         { gpib_lvalp->ival=T300s; return(T_NUMBER);}
T1000s        { gpib_lvalp->ival=T1000s; return(T_NUMBER);}

0[xX][0-9a-fA-F]+ {
	gpib_lvalp->ival = strtol(gpib_yyget_text(yyscanner), NULL, 0);
	return( T_NUMBER );
}

(00)?[0-9]+ {
	gpib_lvalp->ival = strtol(gpib_yyget_text(yyscanner), NULL, 0 );
	return( T_NUMBER );
}

[ \t]

[a-zA-Z0-9_\-]+ {
	gpib_lvalp->sval = gpib_yyget_text(yyscanner);
	/* printf("bare string <%s> should be enclosed in double-quotes\n", gpib_yytext); */
	return (T_STRING);
}

. { return( gpib_yyget_text(yyscanner)[0] ); }

%%

