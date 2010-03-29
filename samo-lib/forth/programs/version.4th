\ display SD Card version

: test-version-sequence  ( -- )
    s" version.txt" r/o open-file ?dup
    if
        ." open error = " dec. drop
        cr
        false exit
    then

    >r \ save fileid

    begin
        here 256 r@ read-line ?dup  \ u2 f ior ior?
        if
            ." read error = " dec. 2drop
            r> close-file drop
            cr
            false exit
        then
    while   \ u2
            here swap    \ c-addr u
            2dup type cr
            s" VERSION:" \ c-addr u c-addr' u'
            2over        \ c-addr u c-addr' u' c-addr u
            >r over      \ c-addr u c-addr' u' c-addr u'  R: u
            dup r> > if
                2drop 2drop 2drop
            else
                compare 0= if
                    0 2 lcd-at-xy
                    lcd-type
                else
                    2drop
                then
            then
    repeat
    drop
    r> close-file drop

    button-flush
    ctp-flush
    key-flush

    ." *VERSION*" cr
    key              \ external process will send Y or N

    [char] Y =       \ return true if Y received
;

: test-version-main ( -- )
    lcd-cls
    s" SD Card Version Test" lcd-type

    test-version-sequence if
        s" PASS"
    else
        s" FAIL"
    then
    lcd-text-columns 2/ lcd-text-rows 2/ lcd-at-xy
    2dup lcd-type
    500000 delay-us
    type ." : version test" cr
;


