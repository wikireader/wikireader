.( ctp line drawing )

base @ decimal

.( ctp-read )

variable ctp-x
variable ctp-y
variable ctp-state


: ctp-read ( -- x y f )
  begin
    ctp-key dup $aa =
    if
      0 ctp-state !
    then
    ctp-state @
    case
      0 of drop 1 ctp-state +! endof
      1 of 7 lshift ctp-x ! 1 ctp-state +! endof
      2 of ctp-x @ or ctp-x ! 1 ctp-state +! endof
      3 of 7 lshift ctp-y ! 1 ctp-state +! endof
      4 of ctp-y @ or ctp-y ! 1 ctp-state +! endof
      5 of ctp-x @ 2/ swap ctp-y @ 2/ swap 1 ctp-state +! exit endof
    endcase
  again
;


.( draw )

variable down

: draw ( -- )
  lcd-clear
  false down !
  begin
    ctp-read
    if
      down @
      if
        lcd-line-to
      else
        lcd-move-to
        true down !
      then
    else
      false down !
    then

    P6_P6D p@ $07 and 0<> if
      lcd-clear
    then
  enough? until
;

draw

.( complete )
base !
