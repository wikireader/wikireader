hi

: .CR . CR ;
: 1+ 1 + ;
: DUP2 OVER OVER ;
: 0= 0 = ;

VARIABLE TOTO 0 TOTO !
: T TOTO @ DUP .CR 1+ TOTO ! ; T T T T

: dupstrlen -1 BEGIN 1+ DUP2 + C@ 0= UNTIL ;
: zTYPE dupstrlen TYPE ;

: dumpargc argc @ .CR ;
: dumpargv argv @ argc @ 1 -
  FOR
  DUP @ zTYPE
  CR CELL+
  NEXT ;
: dumpargs dumpargc dumpargv ;
dumpargs

BYE
