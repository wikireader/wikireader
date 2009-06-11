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
        P6_P6D p@ $07 and
    until
    drop
;

: test-sd
    lcd-cls s" Reading SD card" lcd-type
    dup 0 120 lcd-at-xy s" sector count:" lcd-type
    begin
        0 scan-disk-from
        P6_P6D p@ $07 and
        case
            $02 of   \ left button
            endof
            $04 of   \ centre button
            endof
            $01 of   \ right button
                exit
            endof
        endcase
    again
;

test-sd

.( complete )
base !