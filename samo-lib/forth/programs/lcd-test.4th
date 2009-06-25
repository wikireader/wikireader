\ lcd test patterns
decimal

: eye ( -- )
  \ lcd-cls
  76 0 ?do 75 i -
    120 over - 100 lcd-move-to
    120 over 25 + lcd-line-to
    120 over + 100 lcd-line-to
    120 i 100 + lcd-line-to
    120 over - 100 lcd-line-to
    drop
  5 +loop
;

250000 constant stripe-delay

: stripe ( -- )
    lcd-vram lcd-vram-size $80 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $40 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $20 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $10 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $08 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $04 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $02 fill
    stripe-delay delay-us
    lcd-vram lcd-vram-size $01 fill
;


: charset-test ( -- )
    lcd-cls
    256 0 ?do
        i lcd-emit
    loop
;


: test-lcd-menu ( -- flag )
    button-flush
    ctp-flush
    key-flush

    200000 delay-us
    10 lcd-text-rows 1- lcd-at-xy
    s" PASS         FAIL" lcd-type

    button-flush
    begin
        button? if
            button
            case
                button-left of
                    true exit
                endof
                button-centre of
                endof
                button-right of
                    false exit
                endof
            endcase
        then

        ctp-pos? if
            ctp-flush
        then

        key? if
            key-flush
        then

        wait-for-event
    again
;

: test-lcd-pass-fail ( c-addr u -- flag )
    test-lcd-menu
    if
        s" PASS: "
        true >r
    else
        s" FAIL: "
        false >r
    then
    type type cr
    r>
;

: test-lcd-sequence ( -- flag )
    eye
    s" LCD eye test"
    test-lcd-pass-fail

    lcd-set-all
    s" LCD all black pixels"
    test-lcd-pass-fail
    and

    stripe
    s" LCD moving stripes"
    test-lcd-pass-fail
    and

    charset-test
    s" LCD text"
    test-lcd-pass-fail
    and
;

: test-lcd-main
    lcd-cls
    s" LCD Test" lcd-type
    cr
    test-lcd-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-cls
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    type ." : LCD test" cr
;
