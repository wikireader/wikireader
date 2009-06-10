.( ctp tests )

base @ decimal

.( ctp-read )

variable ctp-x
variable ctp-y

.( ctp-read )

: ctp-read ( -- x y )
  0 0
  begin
    2drop
    begin
      ctp-key $aa =
    until
    ctp-key 7 lshift ctp-key or 2/
    ctp-key 7 lshift ctp-key or 2/
    ctp-key 1 =
  until
;

.( draw )

variable pixel

: draw ( -- )
  lcd-clear
  1 pixel !
  begin
    ctp-read
    pixel @ if
      lcd-set-pixel
    else
      lcd-clear-pixel
    then
    P6_P6D p@ $01 and 0<> if
      lcd-clear
      1 pixel !
    then
    P6_P6D p@ $02 and 0<> if
\      lcd-clear-1
      0 pixel !
    then
  enough? until
;

draw

.( complete )
base !
