.( forth.4th )
base @ decimal


.( case statements - case of endof endcase )

: case ( -- 0 )
  0
; immediate compile-only

: of ( -- <if> )
     compile over
     compile =
     [compile] if
     compile drop
; immediate compile-only


: endof ( <if> -- <else> )
  [compile] else
; immediate compile-only


: endcase ( 0 <if>*n -- )
  compile drop
  begin
    ?dup
  while
    [compile] then
  repeat
; immediate compile-only


.( complete )
base !
