\ scan-disk

base @ decimal

variable error-count

: scan-sector ( u -- f )
    >r here 1024 + 1 r> read-sectors ?dup
    if drop
        0 8 lcd-at-xy s" error count:" lcd-type
        14 8 lcd-at-xy
        1 error-count +! error-count @ lcd-number true
        filesystem-init
    else false then
;

: scan-disk-from ( u -- )
    begin
        dup 14 5 lcd-at-xy lcd-number
        dup scan-sector if drop exit then
        1+
        button?
    until
    drop
;

: test-sd
    key-flush
    ctp-flush
    button-flush
    lcd-cls s" Reading SD card" lcd-type
    dup 0 5 lcd-at-xy s" sector count:" lcd-type
    9 lcd-text-rows 1- lcd-at-xy s" Clear          Exit" lcd-type
    begin
        0 scan-disk-from
        ctp-pos? if ctp-flush then
        key? if key-flush then
        button?
        if
            button
            case
                button-left of
                    0 error-count !
                endof
                button-centre of
                endof
                button-right of
                    exit
                endof
            endcase
        then
    again
;

base !
