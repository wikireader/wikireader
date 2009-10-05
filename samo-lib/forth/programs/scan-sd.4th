\ scan-disk

base @ decimal

variable error-count
variable max-time
variable min-time
128 constant number-of-sectors

0 max-time !
$7fffffff min-time !

: scan-sector ( u -- ticks f )
    timer-read >r
    >r here 1024 + number-of-sectors r> read-sectors timer-read >r ?dup
    if drop
        0 12 lcd-at-xy s" error count:" lcd-type
        14 12 lcd-at-xy
        1 error-count +! error-count @ lcd-number true
        filesystem-init
    else false then
    2r> swap - number-of-sectors / swap
;

: scan-disk-from ( u -- )
    begin
        dup 14 3 lcd-at-xy lcd-number
        dup scan-sector if 2drop exit then
        dup 14 5 lcd-at-xy lcd-number
        dup
        min-time @ min dup min-time !
        14 7 lcd-at-xy lcd-number
        max-time @ max dup max-time !
        14 9 lcd-at-xy lcd-number
        number-of-sectors +
        button?
    until
    drop
;

: test-sd ( -- )
    key-flush
    ctp-flush
    button-flush
    lcd-cls s" Reading SD card" lcd-type
    0 3 lcd-at-xy s" sector count:" lcd-type
    0 5 lcd-at-xy s" sector time:" lcd-type
    0 7 lcd-at-xy s" min. time:" lcd-type
    0 9 lcd-at-xy s" max. time:" lcd-type
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
