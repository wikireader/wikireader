\ bitmap.4th

decimal

: get-file  ( b u -- )
    cr r/o open-file ?dup
    if  cr ." open error = " dec. drop exit
    then

    >r \ save fileid

    begin
        here 30 r@ read-file ?dup  \ u2 ior ior?
        if  cr ." read error = " dec. drop
            r> close-file drop exit
        else
            32 allot
        then
        0=
    until
    r> close-file drop
;



: get ( -- \ <string><space> )
    lcd-cls s" loading..." lcd-type
    bl parse get-file ;


variable top
variable last-x
variable last-y


: home-page ( -- )
    top @ LCDC_MADD p!
;

: scroll
    begin
        ctp-pos? if
            ctp-pos dup 0<
            if
                2drop
                0 last-x !
                0 last-y !
            else
                \ x y
                last-y @ ?dup if
                    swap dup last-y ! -
                    32 * LCDC_MADD p@ +
                    dup top @ <
                    if
                        drop
                    else
                        LCDC_MADD p!
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

\ load image

here top !

get sans.img

.( scroll - using function keys L=down C=home R=up )
LCDC_MADD p@
top @ LCDC_MADD p!
scroll
LCDC_MADD p!
lcd-cls
