#!/usr/bin/wish -f
#
#  This example uses the BLT extension that is provided as binary
#  distribution in the contrib package, it can be obtained from
#  ftp://ftp.neosoft.com/tcl/extensions/BLT2.1.tar.gz
#
#  It is a simple life display measurement program for slow data aquisition
#  that can be changed for own needs.
#  see DoMeas for this.
#
#  If you change this for taking your data please let me know
#  clausi@chemie.fu-berlin.de
#

load ../libgpib_tcl.so
load ../../../contrib/tcltk/modules/libBLT.so.2.1

set auto_path [linsert $auto_path 0 ./tclib]


## init device 

set device [gpib find "voltmeter"]
gpib sic   $device
gpib ren   $device 1
gpib clear $device
gpib write $device "D1Ready"

# setup device
gpib write $device "D0L0Q1OT1A1"


#########################################################################

set setup_file .xsetup

##############################
# Global variables
#
set yval 0

set Status(scan)    0
set Status(point)   0
set Status(mscan)   5
set Status(unsaved) 0
set Status(run)     0
set Status(pause)   0
set Status(from)    0
set Status(to)      50
set Status(delay)   500
set Status(sens)    0
set Status(path)    "./test.dat"
set Status(fnum)    0
set Status(comment) ""
set Status(nref)    5
set Status(delay)   1000

### load setup if one

if [file exists $setup_file] {
    source $setup_file
}

######################################################################

proc ExitGracefully { } {
  global Status setup_file
  if { $Status(unsaved) } {

  } else {
    SaveSetup Status $setup_file	
    destroy .
  }
}

######################################################################
B_Init DataA
B_Init DataB
B_Init DataC


proc Start {} {
  global Status DataA DataB DataC 
  set Status(run) 1
  set Status(pause) 0
  set Status(point) $Status(from)
  set Status(scan) 0
  B_Init DataA
  B_Init DataB
  B_Init DataC

  .graph.xy xaxis configure -min $Status(from) -max $Status(to)
  .labels.t10 configure -background red
  set Status(mscan) [.scales.s get]

  RunTask
}

proc Stop {} {
 global Status
 set Status(run) 0
}

proc Pause {} {
 global Status
 if { $Status(pause) } {
  set Status(pause) 0
  .bbar.p configure -text "Pause"
 } else {
  set Status(pause) 1
  .bbar.p configure -text "Cont"
 }
}

####
# Task Scheduler
####

proc RunTask {} {
  global Status
  if { $Status(run) } {
    if { ! $Status(pause) } { 
       .labels.t4 configure -background blue ; update idletasks
       DoMeas 
       .labels.t4 configure -background grey ; update idletasks
    } 
    after $Status(delay) RunTask
  }
}

####
# Measurement
####

proc DoMeas {} {
  global Status DataA DataB DataC yval device

  if { $Status(point) >= $Status(to) } {

           B_AddY DataA DataB DataC
           DisplayRefresh 


    set Status(point) $Status(from)
    incr Status(scan)
    # After scan save added buffer do meas buffer 
    # and clear temp buffer
    B_Copy DataC DataB
    # write backup file
    B_WriteFile DataC "temp.bak"
    B_Init DataA
  } 
  if { $Status(scan) < $Status(mscan) } {  

#
#  Here the Data String is read from the device
#
# begin read data

    set yval [ gpib read $device 15 ]

#
# end read data
#


    B_Put  DataA $Status(point) $yval
    B_AddY DataA DataB DataC
    DisplayRefresh 

##################
# enable this if you want the plot not to be refreshed on each data point

#    if { [expr $Status(point) % $Status(nref) ] == 0 } { 
#           B_AddY DataA DataB DataC        
#           DisplayRefresh 
#    }
    incr Status(point)
  } else {
    #finished !
#    if { [llength DataA] > 0 } {
#      B_AddY DataA DataB DataC
#    }
#    DisplayRefresh 
    set Status(run) 0
    Save
  }
}

####
# display
####

proc DisplayRefresh { } {
 global Status DataC   
# if { [llength DataC ] > 1 } {
 .graph.xy element configure display -xdata $DataC(x) -ydata $DataC(y)  
 .graph.xy element show display 
# }
}


######
#  Menus and Buttons
######################################################################

frame .labels   -relief ridge -borderwidth 2
pack  .labels   -side top -fill x

label .labels.t1 -text "Scans: "
label .labels.t2 -textvariable Status(scan) -background grey -width 5
pack  .labels.t1 .labels.t2 -side left

label .labels.t3 -text "Point: "
label .labels.t4 -textvariable Status(point) -background grey -width 5
pack  .labels.t3 .labels.t4 -side left

label .labels.t5 -text "="
label .labels.t6 -textvariable yval -background grey -width 15
label .labels.t7 -text " V"

pack  .labels.t5 .labels.t6 .labels.t7 -side left

label .labels.t8 -text "File: "
label .labels.t9 -textvariable Status(path) -background grey
label .labels.t10 -textvariable Status(fnum) -background grey

pack  .labels.t8 .labels.t9 .labels.t10 -side left -padx 5
##############################

frame .cmt
label .cmt.l1 -text Comment
entry .cmt.e1 -relief sunken -background grey90 -textvariable Status(comment)


pack .cmt.l1 -side left
pack .cmt.e1 -side left -fill x -expand yes
pack .cmt -fill x
##############################
frame .graph     -relief ridge -borderwidth 2
pack  .graph

graph .graph.xy -relief sunken -background grey -borderwidth 2 -width 600 -height 400
pack    .graph.xy

.graph.xy element line display -xdata {0 0} -ydata {0 1}  


##############################
frame    .entrys    -relief ridge -borderwidth 2

label    .entrys.l1 -text From
entry    .entrys.e1 -background grey90 -textvariable Status(from)\
                    -relief sunken

pack     .entrys.l1 .entrys.e1

label    .entrys.l2 -text To
entry    .entrys.e2 -background grey90 -textvariable Status(to)\
	            -relief sunken

pack     .entrys.l2 .entrys.e2

pack     .entrys -side left -fill x


##############################
frame  .bbar -relief ridge -borderwidth 2
pack   .bbar -fill x -anchor n

button .bbar.q -text "Quit"  -command "ExitGracefully"
button .bbar.s -text "Start" -command "Start"
button .bbar.o -text "Stop"  -command "Stop"
button .bbar.p -text "Pause" -command "Pause"
button .bbar.sv -text "Save" -command "Save"
pack   .bbar.q .bbar.s .bbar.o .bbar.p .bbar.sv -side left

frame  .scales -relief ridge -borderwidth 2

scale  .scales.s -label "Scans" -from 1 -to 200 \
	-sliderlength 15 -width 20 -showvalue 1 -orient horizontal
	                        

pack   .scales.s -side left -fill x
pack   .scales -side top -fill x

label    .scales.l3 -text Path:
entry    .scales.e3 -background grey90 -textvariable Status(path)\
	            -relief sunken

pack     .scales.l3 -side top -fill x
pack     .scales.e3 -fill x


####
#
####
proc Save { } {
  global DataC DataA DataB Status 

  set fpath $Status(path).$Status(fnum)
    while { [file exists $fpath] } {
	set Status(fnum) [expr $Status(fnum) + 1]
	set fpath $Status(path).$Status(fnum)
    }

  B_WriteFile  DataC $fpath "$Status(comment)"
  .labels.t10 configure -background grey
}

####
#
####

proc SaveSetup { arr fname } {
global $arr 
  set fds [ open $fname "w" ] 

  foreach n [array names $arr ] {
    set val [eval "set $arr\($n\)" ]
    puts $fds "set $arr\($n\) \"$val\" "

  }
  close $fds
}
