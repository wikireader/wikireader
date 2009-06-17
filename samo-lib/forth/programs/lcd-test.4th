\ lcd test patterns
decimal

: eye ( -- )
  lcd-cls
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


: test-menu ( -- flag )
    begin
        P6_P6D p@ $07 and
        0=
    until

    500000 delay-us
    10 lcd-text-rows 1- lcd-at-xy
    s" PASS         FAIL" lcd-type

    begin
        P6_P6D p@ $07 and
        case
            $02 of   \ left button
                true
                exit
            endof
            $04 of   \ centre button
            endof
            $01 of   \ right button
                false
                exit
            endof
        endcase
    again
;

\ test sequence

: test-sequence ( -- )
    eye
    test-menu 0= if
        cr ." FAIL: eye test" cr
        false exit
    then

    lcd-set-all
    test-menu 0= if
        cr ." FAIL: all black" cr
        false exit
    then

    stripe
    test-menu 0= if
        cr ." FAIL: stripe" cr
        false exit
    then

    charset-test
    test-menu 0= if
        cr ." FAIL: text" cr
        false exit
    then

    \ all tests passed
    true
;

: test-lcd
    lcd-cls
    test-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-cls
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    cr type ." : LCD" cr
;

test-lcd
