\ test suspend current

: test-suspend-sequence ( -- flag )
    lcd-cls
    button-flush
    ctp-flush
    key-flush

    s" SUSPEND CURRENT MEASUREMENT" lcd-type

    lcd-cr lcd-cr
    s" 1. Do NOT touch screen" lcd-type lcd-cr
    s" 2. Do NOT press keys" lcd-type lcd-cr
    s" 3. Please wait while" lcd-type lcd-cr
    s"    current is measured" lcd-type lcd-cr

    10000 delay-us

    button-flush
    ctp-flush
    key-flush

    ." *SUSPEND*" cr
    key              \ external process will send Y or N

    [char] Y =       \ return true if Y received

    ." *RESUME*" cr

    button-flush
    ctp-flush
    key-flush
;

: test-suspend-main ( -- )
    lcd-cls
    cr
    test-suspend-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-cls
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    type ." : suspend test" cr
;
