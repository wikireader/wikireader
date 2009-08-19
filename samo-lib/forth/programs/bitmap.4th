\ bitmap.4th

decimal
also c33

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
                        \ begin
                        \     LCDC_PS p@ $80 and
                        \ until
                        LCDC_MADD p!
                        \ 30000 delay-us
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

        button? if
            button
            case
                button-left of
                    home-page
                endof
                button-centre of
                endof
                button-right of
                    exit
                endof
            endcase
        then

        key? if
            key-flush
        then

        \ cannot suspend or LCD image will blank
        \ as image is in SDRAM
        \ wait-for-event
    again
;

\ load image

align  here top !

get sans.img

.( scroll - using function keys L=down C=home R=up )
LCDC_MADD p@
top @ LCDC_MADD p!
scroll
LCDC_MADD p!
lcd-cls
