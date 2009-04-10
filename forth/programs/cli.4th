.( cli.4th - some simple commands )

base @ decimal

\ just some simple commands to work with the file system

: print-file-size ( b u -- )
  r/o open-file ?dup
  if   drop 0
  else dup file-size drop   \ fileid size
       swap close-file drop \ size
  then
  10 .r ;


\                                        -
.( ls                                    - list the root directory )

: ls ( -- )
  cr s" /" open-directory ?dup
  if  cr ." open-directory error = "
      dec. drop exit
  then
  >r    \ save dirid

  begin
    here 256 r@ read-directory ?dup
    if  cr ." directory read error = "
        dec. drop
        r> close-directory drop exit
    then
    dup
  while
    here swap 2dup print-file-size
    space type cr
  repeat
  drop
  r> close-directory drop ;

\                                        -
.( dir                                   - list the root directory )
: dir ( -- ) ls ;


\                                        -
.( display <file>                        - display a text file on the console )

: display-file  ( b u -- )
  cr r/o open-file ?dup
  if  cr ." open error = " dec. drop exit
  then

  >r \ save fileid

  begin
    here 256 r@ read-line ?dup  \ u2 f ior ior?
    if  cr ." read error = " dec. 2drop
        r> close-file exit
    then
  while   \ u2
    here swap type cr
  repeat
  drop
  r> close-file drop ;


\ display a specific file

: display ( -- \ <string><space> ) bl parse display-file ;


\                                        -
.( delete <file>                         - erase a file )

: delete ( -- \ <string><space> ) bl parse delete-file
  ?dup if  cr ." delete error = " dec. then ;


\                                        -
.( rm <file>                             - erase a file )

: rm delete ;


\                                        -
.( rename <file-old> <file-new>          - change the name of a file )

: rename ( -- \ <string><space><string><space )
  bl parse bl parse rename-file
  ?dup if  cr ." rename error = " dec. then ;


\                                        -
.( mv <file-old> <file-new>              - change the name of a file )

: mv rename ;


\                                        -
.( mkfile <file>                         - create a text file with dummy data )

: (wrl) ( b u fileid -- )
  >r r@
  write-file ?dup if  cr ." write error = " dec. then
  drop
  pad [ctrl] j over c!
  1 r> write-file  ?dup if  cr ." write error = " dec. then
  drop ;


: mkfile ( -- \ filename )
  bl parse w/o create-file ?dup
  if  cr ." create error = " dec. drop exit
  then
  >r \ save fileid

  s" this is the first line of text" r@ (wrl)
  s" this is the second line of text" r@ (wrl)
  s" this is the third line of text" r@ (wrl)
  s" 0123456789" r@ (wrl)
  s" abcdefghijklmnopqrstuvwxyz" r@ (wrl)

  r> close-file drop ;


\                                        -
.( stress <file>                         - create a big text file with dummy data )

: create-big-file ( b u -- )
  w/o create-file ?dup
  if  cr ." create error = " dec. drop exit
  then
  >r \ save fileid
  100 begin
    1000 begin
      s" 0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" r@ (wrl)
      1- ?dup while
    repeat
    r@ flush-file drop
    [char] . emit
    1- ?dup while
  repeat
  r> close-file drop ;

: stress ( -- \ filename )
  bl parse 2dup delete-file drop
    create-big-file ;

.( complete )
base !
