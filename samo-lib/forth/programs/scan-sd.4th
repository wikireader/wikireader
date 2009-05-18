.( scan-disk )
include lcd.4th

base @ decimal

: scan-sector ( u -- f )
  >r here 1024 + 1 r> read-sectors ?dup
  if
     lcd-cls s" rc =" lcd-type lcd-number true
  else false then
;

: scan-disk-from ( u -- )
  lcd-cls s" Reading SD card" lcd-type
  begin
    dup 100 120 lcd-at-xy lcd-number
    dup scan-sector if drop exit then
    1+
  again
  drop
;

0 scan-disk-from
