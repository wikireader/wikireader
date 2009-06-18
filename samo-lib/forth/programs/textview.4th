\ textview.4th

decimal

: get-text-file  ( b u -- )
    cr r/o open-file ?dup
    if  cr ." open error = " dec. drop exit
    then

    >r \ save fileid

    begin
        here 30 blank
        here 30 r@ read-line ?dup  \ u2 f ior ior?
        if  cr ." read error = " dec. 2drop
            r> close-file drop exit
        then
    while   \ u2
            drop
            \ here swap type cr
            here
            30 0 ?do
                dup c@ bl < if
                    dup bl swap c!
                then
                char+
            loop
            drop
            30 allot
    repeat
    drop
    r> close-file drop
;


: get-text ( -- \ <string><space> )
    lcd-cls s" loading: " lcd-type
    bl parse 2dup lcd-type get-text-file ;


variable top
variable last-x
variable last-y
variable cursor-t
variable cursor-b

: home-page ( -- )
    lcd-cls
    top @ dup cursor-t !
    dup cursor-b !
    30 16 * >r r@ lcd-type
    r> cursor-b +!
;

: next-line ( -- )
    lcd-scroll
    0 lcd-height-lines font-height - lcd-move-to
    cursor-b @ 30 lcd-type
    30 dup cursor-t +! cursor-b +!
;

: prev-line ( -- )
    cursor-t @ top @ <> if
        lcd-scroll>
        0 0 lcd-move-to
        cursor-t @ 30 - dup cursor-t !
        30 lcd-type
        -30 cursor-b +!
    then
;

: scroll
    home-page
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop 0 last-y ! 0 last-x !
            else
                last-y @ if
                    >r \ y
                    last-y @ r@ - dup abs font-height >
                    if
                        0<
                        if
                            prev-line
                        else
                            next-line
                        then
                        r> last-y !
                    else
                        drop r> drop
                    then
                else
                    last-y !
                then
                last-x @ if
                    >r \ x
                    last-x @ r@ - dup abs 150 >
                    if
                        0<
                        if
                            15 0 ?do next-line loop
                        else
                            home-page
                        then
                        r> last-x !
                    else
                        drop r> drop
                    then
                else
                    last-x !
                then
            then
        then

        P6_P6D p@ $07 and
        case
            $02 of   \ left button
                home-page
            endof
            $04 of   \ centre button
            endof
            $01 of   \ right button
                exit
            endof
        endcase
    again
;


here top !

get-text tf.txt

.( scroll - using function keys L=down C=home R=up )
scroll
