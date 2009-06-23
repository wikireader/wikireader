#!/usr/bin/wish -f
#
# This example is a simple Digital Voltmeter display
#
#
#

load ../libgpib_tcl.so

#
# init device
#---------------------------------------------
 
set device [gpib find voltmeter ]

gpib sic   $device
gpib ren   $device 1
gpib clear $device
gpib write $device "D1Ready"
gpib write $device "D0L0Q1OT1A1"
#gpib write $device "D0L0Q1OT2"

#---------------------------------------------
#

set result [gpib read $device 15]

set mode "V"
set refresh 50
set block 0

proc displ_refresh {} {
	global result device refresh block mode

	gpib wait $device srq


	.light configure -background red
	update
        set result [gpib read $device 15 ]$mode
        
	if { $block == 0 } { 
		after $refresh displ_refresh
        }
	.light configure -background steelblue
	update
}

proc Mode { mod } {
 global mode
 set mode $mod
}


label  .lab  -text "GPIB Demo" -font -adobe-times-bold-r-normal--18-180-75-75-p-99-iso8859-1 -justify right
label .light -width 1 -height 1 -relief raised -text "*"

label  .val  -width 15 -textvariable result -relief sunken -font -adobe-helvetica-bold-r-normal--24-240-75-75-p-138-iso8859-1

button .but1 -text "Quit" -command { set block 1; update idletasks ;gpib close $device ; exit}
button .but2 -text "Ohm"  -command { gpib write $device "O2" ; Mode "Ohm" }
button .but3 -text "V ~"   -command { gpib write $device "VA" ; Mode "V" }
button .but4 -text "V ="   -command { gpib write $device "VD" ; Mode "V" }
button .but5 -text "A ~"   -command { gpib write $device "IA" ; Mode "A" }
button .but6 -text "A ="   -command { gpib write $device "ID" ; Mode "A" }
button .but7 -text "AutoRange"  -command { gpib write $device "A1" }
button .but8 -text "NoAuto" -command { gpib write $device "A0" }


pack append . .lab { top fillx} .val { top fillx } .light {right fillx }  
pack append . .but1 { bottom fillx } .but2 { bottom fillx } .but3 { bottom fillx } \
              .but4 { bottom fillx} .but5 { bottom fillx} .but6 { bottom fillx} \
	      .but7 { top filly } .but8 { top filly }

displ_refresh










