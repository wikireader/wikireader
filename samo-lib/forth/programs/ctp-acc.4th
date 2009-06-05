.( display and ctp accuracy )

base @ decimal

.( ctp-read )

variable ctp-x
variable ctp-y


: ctp-read ( -- x y )
  0 0
  begin
    2drop
    begin
      ctp-key $aa =
    until
    ctp-key 8 lshift ctp-key or 2/
    ctp-key 8 lshift ctp-key or 2/
    ctp-key 1 =
  until
;

.( function keys )

: get-function-key ( -- n )
  P6_P6D p@ 7 and ;


.( ctp-accuracy )

: ctp-accuracy ( -- )
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
