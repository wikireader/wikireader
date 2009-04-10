.( create names for all registers )

base @ decimal

\ handle assembler comments
: ;;; ( -- ) #tib @ >in ! ; immediate

\ REGDEF 0x1234 16 IO_REG
: REGDEF ( -- \ <string> <string> <string> )
  base @

  bl parse 2 - >r 2 + pad r@ over c!
  1+ r> cmove pad hex number? 0=
  if  drop  base !
      abort" number required
  then
  \ base addr
  bl parse >r pad r@ over c!
  1+ r> cmove pad decimal number? 0=
  if  drop  base !
      abort" number required
  then
  \ base addr bits
  create , ,
  base ! [char] . emit ;

\ fetch a peripheral register value
: p@ ( reg-addr -- value )
  2@ \ address size
  dup 32 = if  @
          else 16 = if    h@
                    else  c@
                    then
          then
;

\ display peripheral register
: p? ( reg-addr -- )  p@ . ;


\ store a value to a peripheral register
: p! ( value reg-addr -- )
  2@ \ address size
  dup 32 = if  !
          else 16 = if    h!
                    else  c!
                    then
          then
;

.( read register definitions file )
base !
include c33regs.inc
