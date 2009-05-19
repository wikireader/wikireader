.( display and ctp accuracy )
include lcd.4th
include regs.4th

base @ decimal


$01 constant RDBFx


9600 constant ctp-bps

48000000 constant cpu-clock
8 constant divmd
cpu-clock divmd /
ctp-bps 2* / 1-
constant ctp-brtrd

.( ctp-init )

: ctp-init ( -- )
  $c3 EFSIF1_CTL p!     \ 8-bit async, no parity, internal clock, 1 stop bit
  $10 EFSIF1_IRDA p!    \ DIVMD = 1/8, General I/F mode
  ctp-brtrd dup EFSIF1_BRTRDL p!
  256 / EFSIF1_BRTRDM p!
  $01 EFSIF1_BRTRUN p!  \ enable
  $01 P0_47_CFP p!      \ rx only ( tx => + 4 )
;


.( ctp-ready )

: ctp-ready ( -- f )
  EFSIF1_STATUS p@ RDBFx and 0<>
;

.( ctp-get )

: ctp-get ( -- w )
  begin ctp-ready until
  EFSIF1_RXD p@
;


.( ctp-read )

variable ctp-x
variable ctp-y


: ctp-read ( -- x y )
  0 0
  begin
    2drop
    begin
      ctp-get $aa =
    until
    ctp-get 8 lshift ctp-get or 2/
    ctp-get 8 lshift ctp-get or 2/
    ctp-get 1 =
  until
;

.( function keys )

: get-function-key ( -- n )
  P6_P6D p@ 7 and ;


.( ctp-accuracy )

: ctp-accuracy ( -- )
  ctp-init
  lcd-cls
  s" CTP Accuracy Testing" lcd-type

    0  67 lcd-at-xy s"  67" lcd-type
    0 135 lcd-at-xy s" 135" lcd-type

   80 160 lcd-at-xy s" 79" lcd-type
  160 160 lcd-at-xy s" 159" lcd-type

   79  67 lcd-set-point
  159  67 lcd-set-point
   79 135 lcd-set-point
  159 135 lcd-set-point

  begin
    ctp-read >r >r
    20 32 lcd-at-xy r> lcd-number s"   " lcd-type r> lcd-number
  enough? until
;

ctp-accuracy

.( complete )
base !
