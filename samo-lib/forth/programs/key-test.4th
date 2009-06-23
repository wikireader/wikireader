\ function key tests

base @ decimal


10 constant pos-1
7 dup pos-1 + dup
constant pos-2
+ constant pos-3

: show-state ( flag pos -- )
    lcd-text-rows 1- lcd-at-xy
    if  \ pad with spaces to make strings the same length
        s" ON "
    else
        s" OFF"
    then
    lcd-type
;


: keys-test-menu ( -- )
    lcd-cls
    s" KEY TESTS" lcd-type
    false pos-1 show-state
    false pos-2 show-state
    false pos-3 show-state
    begin
        P6_P6D p@ $07 and
        0=
    until

    begin
        0 10 lcd-at-xy
        P6_P6D p@ $07 and lcd-number

        P6_P6D p@ $02 and
        pos-1 show-state

        P6_P6D p@ $04 and
        pos-2 show-state

        P6_P6D p@ $01 and
        pos-3 show-state

        P6_P6D p@ $07 and $07 =
    until
;

base !
