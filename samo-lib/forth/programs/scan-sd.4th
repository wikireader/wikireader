.( scan-disk )

base @ decimal

variable error-count

: scan-sector ( u -- f )
  >r here 1024 + 1 r> read-sectors ?dup
  if drop
     0 60 lcd-at-xy s" error count:" lcd-type
     120 60 lcd-at-xy 1 error-count +! error-count @ lcd-number true
     filesystem-init
  else false then
;

: scan-disk-from ( u -- )
  begin
    dup 120 120 lcd-at-xy lcd-number
    dup scan-sector if drop exit then
    1+
  again
  drop
;

: test-sd
  lcd-cls s" Reading SD card" lcd-type
  dup 0 120 lcd-at-xy s" sector count:" lcd-type
  begin
    0 scan-disk-from
  again
;

test-sd

.( complete )
base !