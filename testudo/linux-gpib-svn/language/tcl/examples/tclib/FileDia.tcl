
#
# File Dialog routines
#

set File(path) "./data/"
set File(base) "test"
set File(num)  1
set File(result) 0

proc GetFile { { number $File(num) } } {
 global File
 toplevel .f 

 label .f.l -text "File Selector" -relief ridge -borderwidth 2
 pack  .f.l -side top -fill x

 frame .f.e -relief ridge -borderwidth 2
 label .f.e.l1 -text "Basename" 
 entry .f.e.e1 -relief sunken -background grey90 -textvariable File(base) -width 20
 pack  .f.e.l1 -side left  
 pack  .f.e.e1 -side left -fill x

 selector .f.e File(num) "File Number"   
 pack  .f.e -side top -fill both

 frame .f.b -relief ridge -borderwidth 2

 button .f.b.q -text "Cancel" -command "set File(result) 0;destroy .f"
 button .f.b.o -text "OK"     -command "set File(result) 1;destroy .f"
 
 pack  .f.b.q -side left -padx 20 -pady 10 
 pack  .f.b.o -side right -padx 20 -pady 10
 pack  .f.b   -side top -fill x
 
}
######################################################################
proc selector { w var label } {
global $var bitmap_path


frame  $w.frame$var        -relief ridge -borderwidth 2 
button $w.frame$var.up     -text + -command "incr $var 1"
button $w.frame$var.down   -text - -command "incr $var -1"
label  $w.frame$var.disp   -background grey90 -textvariable $var -width 2 \
                           -relief sunken
label  $w.frame$var.label  -text $label 

pack   $w.frame$var -padx 8 -pady 5 -side left -fill x -expand yes
pack   $w.frame$var.down   -side left -padx 8 -pady 8
pack   $w.frame$var.disp   -side left  -pady 8
pack   $w.frame$var.up     -side left -padx 8 -pady 8
pack   $w.frame$var.label  -side left  -expand yes -padx 5


}

#GetFile