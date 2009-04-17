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
        r> close-file drop exit
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

: (wrl) ( b u fileid -- f )
  >r r@ write-file ?dup \ u2 ior ior?
  if  cr ." write error = " dec.
      r> 2drop false exit
  then
  drop
  pad [ctrl] j over c!
  1 r> write-file ?dup \ u2 ior ior?
  if  cr ." write error = " dec.
      drop false exit
  then
  drop true ;


: mkfile ( -- \ filename )
  bl parse w/o create-file ?dup
  if  cr ." create error = " dec. drop exit
  then
  >r \ save fileid

  s" this is the first line of text" r@ (wrl) drop
  s" this is the second line of text" r@ (wrl) drop
  s" this is the third line of text" r@ (wrl) drop
  s" 0123456789" r@ (wrl) drop
  s" abcdefghijklmnopqrstuvwxyz" r@ (wrl) drop

  r> close-file drop ;


\                                        -
.( spinner                               - draw a spinner )

variable spin-pos

\ display spinner + a number (updated when mod == 0)

: spin-char ( u -- u' c )
  case
    0 of 1 [char] - endof
    1 of 2 [char] \ endof
    2 of 3 [char] | endof
    3 of 0 [char] / endof
    0 [char] *
  endcase
;


: spinner ( number modulo -- )
  13 emit \ back to start of line
  spin-pos @ spin-char emit spin-pos !
  over swap mod 0= if 8 u.r else drop then
;


\                                        -
.( scan <file>                           - read a file, no output )

: scan-file  ( b u -- )
  cr r/o open-file ?dup
  if  cr ." open error = " dec. drop exit
  then

  >r \ save fileid

  -1
  begin
    1+
    here 1024 r@ read-file ?dup \ u2 ior ior?
    if  cr ." read error = " dec. drop
        r> close-file drop
        cr ." 1kB blocks =" .
        exit
    then
  while
    [char] . emit
  repeat
  cr ." 1kB blocks =" .
  r> close-file drop ;


\ scan a specific file

: scan ( -- \ <string><space> ) bl parse scan-file ;


\                                        -
.( stress <file>                         - create a big text file with dummy data )

100000 constant stress-level

: create-big-file ( b u -- )
  w/o create-file ?dup
  if  cr ." create error = " dec. drop exit
  then
  >r \ save fileid
  cr
  stress-level begin
    s" 0123456789!@#$%^&*()-_=+[]{};;:,./?><`~|\ abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" r@ (wrl)
    0= if r> close-file drop
       cr ." counts =" dec. dec. exit
    then
    dup 100 spinner
    r@ flush-file drop
    1- ?dup while
  repeat
  r> close-file drop ;

: stress ( -- \ filename )
  bl parse 2dup delete-file drop
    create-big-file ;


\                                        -
.( scan-disk                             - read absolute sectors )

: dump-sector ( u -- )
  >r here 1024 + 1 r> read-sectors
  cr ." rc =" .
  here 1024 + 511 dump
;

: scan-sector ( u -- f )
  >r here 1024 + 1 r> read-sectors ?dup
  if cr ." rc =" . true else false then
;


: dump-disk-from ( u -- )
  begin
    dup cr ." sector =" .
    dup dump-sector
    1+
  enough? until
  drop
;

: scan-disk-from ( u -- )
  cr
  begin
    dup 100 spinner
    dup scan-sector if drop exit then
    1+
  enough? until
  drop
;

: scan-disk ( -- )
  0 scan-disk-from ;



\                                        -
.( write-test                            - write absolute sectors )

8 constant write-count
512 constant sector-size

: buffer-1 ( -- b ) here 1024 + ;
: buffer-2 ( -- b ) here 1024 +
   write-count sector-size * +  ;


: write-sector ( u -- f )
  >r
  buffer-1 write-count r@ read-sectors ?dup
  if cr ." read rc =" .
     r> drop
     true exit
  then
  buffer-1 write-count r@ write-sectors ?dup
  if cr ." write rc =" .
     r> drop
     true exit
  then
  buffer-2 write-count r@ read-sectors ?dup
  if cr ." verify1 rc =" . cr
     buffer-2 write-count r@ read-sectors ?dup
     if cr ." verify2 rc =" .
        r> drop
        true exit
     then
  then
  buffer-1 buffer-2 write-count mem= 0=
  if cr ." verify failed"
     r> drop
     true exit
  then
  r> drop
  false
;

: write-test-from ( u -- )
  cr
  begin
    dup 100 spinner
    dup write-sector if drop exit then
    1+
  enough? until
  drop
;

: write-test ( -- )
  0 write-test-from
;


.( complete )
base !
