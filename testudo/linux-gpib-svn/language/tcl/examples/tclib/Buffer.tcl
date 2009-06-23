######################################################################
#
#@---- Buffer.tcl  Data Buffer manipulation routines
# 
#
######################################################################

####
#@  B_Init { name }  initialize new Buffer or clear old one
####

proc B_Init { name } {
  upvar $name F
  
  set F(x) {}
  set F(y) {}
  set F(i) 0
}

####
#@  B_Dim { buf } returns the actual length of a buffer
####

proc B_Dim { buf } {
  upvar $buf F
  return [llength $F(x)]	
}

####
#@  B_WriteFile { Buf File } writes buffer to file 
####

proc B_WriteFile { Buf File { comment "none" } } {
  upvar $Buf A
  set fd [open $File "w"]
  puts $fd "\#f XY(raw)"
  puts $fd "\#c $comment"
  for { set i 0 } { $i < $A(i) } { incr i } {
    puts -nonewline $fd [lindex $A(x) $i]
    puts -nonewline $fd "      "   
    puts            $fd [lindex $A(y) $i]	
  }
  close $fd
}

####
#@ B_Copy { A B } Copy Buff A to B (dim A is significant)
####

proc B_Copy { BufA BufB } {
  upvar $BufA A $BufB B
  B_Init B
  set B(x) $A(x)
  set B(y) $A(y)
  set B(i) $A(i) 
}

####
#@  B_Put { Buf X Y } Adds a pair X,Y to buffer
####

proc B_Put { Buf X Y } {
  upvar $Buf A

  lappend A(x) $X 
  lappend A(y) $Y
  incr    A(i)

}

####
#@  B_AddY { A B C } Performs an addition of the Y elements of two Buffers
####

proc B_AddY { A B C } {
  upvar $A A1 $B A2 $C C1
  B_Init C1
  for { set i 0 } { $i < $A1(i) } { incr i } {
    	lappend C1(x) [lindex $A1(x) $i]
        if { $A2(i) > $i } {  
	  lappend C1(y) [expr ([lindex $A1(y) $i] + [lindex $A2(y) $i]) / 2.0 ]
        } else {
	  lappend C1(y) [lindex $A1(y) $i]	
        }
  }
  set C1(i) $i
}


######################################################################

