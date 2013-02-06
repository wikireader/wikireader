\ function key tests

base @ decimal


9 constant pos-1
7 dup pos-1 + dup
constant pos-2
+ constant pos-3

: show-message ( flag pos -- )
    lcd-text-rows 1- lcd-at-xy
    case
        0 of s"        " endof
        1 of s" Press  " endof
        2 of s" Release" endof
    endcase
    lcd-type
;

variable keys-timeout

25000 constant debounce-delay
1000 constant millisec

30000 constant max-key-wait

: test-key-button ( -- u )
    0 keys-timeout !
    begin
        ctp-pos? if
            ctp-flush
        then
        key? if
            key-flush
        then

        button? if
            button
            debounce-delay delay-us
            begin
                button?
            while
                    drop button
            repeat
            exit
        then

        millisec delay-us
        1 keys-timeout +!
        keys-timeout @ max-key-wait >
        if
            -1 \ all normal keys are positive
            exit
        then
    again
;

: check-button ( c-addr u c-addr u button -- flag )
    test-key-button = dup >r
    if
        ." PASS: "
    else
        ." FAIL: "
    then
    type ."  button " type cr
    r>
;

: test-keys-stage-1 ( -- flag )
    1 pos-1 show-message
    s" pressed" s" left" button-left check-button
    0 pos-1 show-message
    dup
    if
        2 pos-1 show-message
        s" released" s" left" button-none check-button
        0 pos-1 show-message
        and
    then
;

: test-keys-stage-2 ( -- flag )
    1 pos-2 show-message
    s" pressed" s" centre" button-centre check-button
    0 pos-2 show-message
    dup
    if
        2 pos-2 show-message
        s" released" s" centre" button-none check-button
        0 pos-2 show-message
        and
    then
;

: test-keys-stage-3 ( -- flag )
    1 pos-3 show-message
    s" pressed" s" right" button-right check-button
    0 pos-3 show-message
    dup
    if
        2 pos-3 show-message
        s" released" s" right" button-none check-button
        0 pos-3 show-message
        and
    then
;

: test-keys-sequence ( -- flag )
    lcd-cls
    button-flush
    ctp-flush
    key-flush

    s" KEY TESTS" lcd-type

    lcd-cr lcd-cr
    s" Press and release each key in" lcd-type
    s" sequence as indicated by the" lcd-type
    s" prompts above the keys." lcd-type

    test-keys-stage-1
    test-keys-stage-2 and
    test-keys-stage-3 and
;

: test-keys-main ( -- )
    lcd-cls
    test-keys-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-cls
    s" Key Test" lcd-type
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    type ." : KEY test" cr
;

base !
