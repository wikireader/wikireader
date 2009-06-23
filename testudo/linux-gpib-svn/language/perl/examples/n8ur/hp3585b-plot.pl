#!/usr/bin/perl
#
# hp3585b-plot.pl
# version 0.8 -- 23 June 2003
#
# Plot display of HP 3585B Spectrum Analyzer
# 
# Copyright 2003 by John R. Ackermann  N8UR (jra@febo.com)
# Licensed under the GPL version 2 or later; see the file COPYING
# included with this distribution.  I request, but do not require, that
# any modifications that correct bugs or errors, or increase the program's
# functionality, be sent via email to the author at the address above.
#
# Current status:
# Version 0.8 -- no code changes, but now licensed under the GPL.
#
# Version 0.7 -- works pretty well.  Ability to plot "B" trace not yet
# implemented, and may never be.  If you resize the image from the
# default, the text strings probably will not line up properly, so use
# that feature at your own risk.
#

#----------
# Left and center footers; there's also a timestamp in the lower right corner
my $left_footer = "HP 3585B \@ N8UR";
my $center_footer = "jra\@febo.com";

use strict;
use POSIX qw(setsid);
use Getopt::Std;
use Time::HiRes qw(usleep time gettimeofday);
use LinuxGpib;
use GD;
use GD::Text;
use GD::Graph::mixed;
use GD::Graph::colour;
use Number::Format;
use n8ur qw(trim collapse squash round);
use hp3585b qw(make_trace get_settings);

my $j;
my $reading;
my $tracedata;
my @tmp_trace;
my @traceA;
my @min_traceA;
my @max_traceA;
my $command;
my $last_cal;
my $time_now;
my $counter = 1;
my $gpib_status;

#----------
# usage and option initialization
my $opt_string = 'bdhmr:c:f:t:x:y:v:';
sub usage() {
print STDERR << "EOF";

usage: $0 [-h] [-b] [-d <device>] [-c 1..4] [-r] [[-v num_sweeps [-m]]
	  [-x pixels -y pixels] [-t title]  -f filename

This program collects data from an HP 8569B spectrum analyzer and generates
a PNG format graphic mimicking the the instrument's display.  Most of the
options affect the output format.

By using the -c and -r options, multiple charts can be created that will
overlay nicely on one another.  The -r option not only makes the image
background transparent, it suppresses the timestamp, so the timestamp of
the underlying non-transparent image will not be blurred.

The -v option switches the analyzer to single-sweep mode and triggers 
num_sweeps sweeps, the results of which are averaged into a single display.  
Adding the -m option to the -v option results in a display which shows the 
minimum and maximum values captured as well as the average.

-h	: this (help) message

-b	: display both traces; default is trace A only

-c 	: use alternate 1 through 4 for trace colors; default is 1 (red,blue)

-d	: device name (from /etc/gpib.conf); default is "hp8569b"

-r	: make output file tRansparent and suppress timestamp; default is no  

-t	: optional title (1 line max)

-x,-y	: output dimension in pixels; default 550x450 

-v	: video averaging over numsweeps (an integer from 1 to whatever)
	-m : show minimum and maximum values as well as average

-f	: filename for output PNG

EOF
}

#----------------------
getopts( "$opt_string", \my %opt ) or usage() and exit;
usage() and exit if $opt{h};
usage() and exit if !$opt{f};

# set variables to command line params
my $picfile = $opt{f};

my $both = 0;
if ($opt{b}) {
	$both = 1;
	}

my $colorset = 1;
if ($opt{c}) {
	$colorset = $opt{c};
	}

my $device = "hp3585b";
if ($opt{d}) {
	$device = $opt{d};
	}

my $transparent = 0;
if ($opt{r}) {
	$transparent = 1;
	}

# title
my $title = "";
if ($opt{t}) {
	$title = $opt{t};
	}

# x size
my $x = 550;
if ($opt{x}) {
	$x = $opt{x};
	}

# y size 
my $y = 450;
if ($opt{y}) {
	$y = $opt{y};
	}

# video averaging
my $num_sweeps = 0;
if ($opt{v}) {
	$num_sweeps = $opt{v};
	}

my $min_max = 0;
if ($opt{m}) {
	$min_max = 1;
	}

# set up picfile
open (PIC, ">$picfile") ||
	die "Can't open image file $picfile!\n";

# initialize instrument
my $dev = LinuxGpib::ibfind($device) ||
	die "Can't open device $device!\n";

#----------
my ($start_freq,$center_freq,$stop_freq,$bin_freq,$top_right_pre,
        $top_right_val,$top_right_suf,$marker_freq,$marker_val,$rbw_val,
        $vbw_val,$db_div_val,$sweep_val,$sweep_suf,$range_val,$range_suf,
        $ref_val,$ref_suf,$ref_bottom,$span_val) = get_settings($dev);

#----------
# get data
if ($num_sweeps == 0) {
	# get trace data - normal mode
	print "Getting trace in normal mode...\n";
	$command = "TS5BD5";
	LinuxGpib::ibwrt($dev,$command,length($command));
	LinuxGpib::ibrd($dev,$tracedata,2004);
	@traceA = make_trace($tracedata);
	}
else {
	# get trace data - video averaging mode
	# set nonbuffered mode
	select(STDOUT), $| = 1;
	print "Generating Video Average over ",$num_sweeps,
		" sweeps (",$sweep_val," ",$sweep_suf,"/sweep):\n";
	# set timeout; we get some long sweep times
	# 15 = 100s, 16 = 300s, 17 = 1000s
	LinuxGpib::ibtmo($dev,16);
	usleep(10000);
	
	# clear trace and set single sweep with operation-complete interrupt
	$command = "CACQ";
	LinuxGpib::ibwrt($dev,$command,length($command));
	usleep(10000);
	$command = "S2";
	LinuxGpib::ibwrt($dev,$command,length($command));
	usleep(10000);
	# initiate a calibration and set last cal time
	$command = "AC1";
	$last_cal = time;
	LinuxGpib::ibwrt($dev,$command,length($command));
	while ($counter <= $num_sweeps) {
		$time_now = time;
		if (($time_now - $last_cal) > 90) {
			# initiate a calibration and set last cal time
			$command = "AC1";
			LinuxGpib::ibwrt($dev,$command,length($command));
			$last_cal = time;
			}	

		# poll status
		LinuxGpib::ibrsp($dev,$gpib_status);
		# trigger a sweep
		$command = "S2";
		LinuxGpib::ibwrt($dev,$command,length($command));
		usleep(175000);
		# wait until SRQ
		$gpib_status = LinuxGpib::ibwait($dev,(0x800 | 0x4000));
		if ($gpib_status & 0x4000) { print "Failure: TIMO\n ";}
		# get reading
		$command = "TS5BD5";
		LinuxGpib::ibwrt($dev,$command,length($command));
		LinuxGpib::ibrd($dev,$tracedata,2004);
		@tmp_trace = make_trace($tracedata);
		for ($j=0;$j<=1000;$j++) { 
			$traceA[$j] += $tmp_trace[$j];
			if ($counter == 1) {
				$min_traceA[$j] = $tmp_trace[$j];
				$max_traceA[$j] = $tmp_trace[$j];
				}
			else {
				if ($tmp_trace[$j] < $min_traceA[$j]) {
					$min_traceA[$j] = $tmp_trace[$j];
					}
				if ($tmp_trace[$j] > $max_traceA[$j]) {
					$max_traceA[$j] = $tmp_trace[$j];
					}
				}
			 }
		$counter++;
		print ".";
	}
	# set back to continuous sweep and clear trace
	$command = "S1CA";
	LinuxGpib::ibwrt($dev,$command,length($command));
	print "\n\n";
}

# convert trace values to dBm if necessary
for ($j=0;$j<=1000;$j++) {
	if ($num_sweeps) {
		$traceA[$j] /= $num_sweeps;
		}
	if ($ref_suf eq "dBm") {
		$traceA[$j] += 13;
		$min_traceA[$j] += 13;
		$max_traceA[$j] += 13;
		}
	}

# create x axis labels and marker plot
my @x_array = ();
my @marker_array = ();
my $seed = $start_freq;
for ($j = 0; $j <= 1000; $j++) {
	$x_array[$j] = $seed;
	if ($marker_freq == $seed) {
		if ($num_sweeps) {
			$marker_val = trim(sprintf("%5.2f",$traceA[$j]));
			}
		$marker_array[$j] = $marker_val;
		}
	else {
		$marker_array[$j] = undef;
		}
	$seed = round(4,($seed + $bin_freq));
	}

# create annotation strings for plotting
if ($db_div_val == 10) {
	$marker_val = trim(sprintf("%5.1f",round(1,$marker_val)));
	}
else {
	$marker_val = trim(sprintf("%5.2f",round(2,$marker_val)));
	}

my $numfmt = new Number::Format(-thousands_sep => ' ', -decimal_point => '.');
my $annotation1= $top_right_pre . ": " .
	$numfmt->format_number($top_right_val) . " Hz, " .
	$marker_val . " " . $ref_suf;
my $annotation2=
	"RBW: " . $rbw_val . " Hz  VBW: " . $vbw_val . " Hz  Scale: " .
	 $db_div_val .  " dB/  Sweep: " . $sweep_val . " Sec.  Range: " .
	 $range_val . " " . $range_suf;

# plotting routines

# chart types and colors; this is so the marker axis stays in the 
# right place and is plotted last to overwrite the trace
my @chart_set;
my @cset;
if ($both) {
	@chart_set = [qw(lines lines points)];
	@cset = [qw(lred blue green green)];
	if ($colorset == 2) {
		@cset = [qw(lblue lred black)];
		}
	if ($colorset == 3) {
		@cset =  [qw(green purple black)];
		}
	if ($colorset == 4) {
		@cset = [qw(purple green black)];
		}
	}
else {
	if ($min_max) {
		@cset = [qw(lred green green black)];
		@chart_set = [qw(lines lines lines points)];
		}
	else {
		@cset = [qw(lred black)];
		@chart_set = [qw(lines points)];
		}
		
	if ($colorset == 2) {
		@cset = [qw(lblue black)];
		}
	if ($colorset == 3) {
		@cset =  [qw(green black)];
		}
	if ($colorset == 4) {
		@cset = [qw(purple black)];
		}
	}
	
# set some dimensions
my $l_margin = 15;
my $r_margin = 25;
my $t_margin = 50;
my $b_margin = 35;

# format the x axis to nothing at all
sub x_format {
	return "";
	}

my $spec_display = new GD::Graph::mixed($x,$y);

$spec_display->set (
	types => @chart_set,
	dclrs => @cset,
	markers => [2,2,2,2,2,2,2],
	marker_size => 3,
	l_margin => $l_margin,
	r_margin => $r_margin,
	t_margin => $t_margin,
	b_margin => $b_margin,
	transparent => $transparent,
	y_max_value => $ref_val,
	y_min_value => $ref_bottom,
	y_tick_number => 10,
	y_long_ticks => 1,
	x_number_format => \&x_format,
	x_min_value => $start_freq,
	x_max_value => $stop_freq,
	x_tick_number => 10,
	x_label_skip => 5,
	x_long_ticks => 1
	);

my $foo;
#if ($both) {
#	$foo = $spec_display->plot([\@x_array,\@traceA,
#		\@traceB,\@marker_array]);
#	}
#else
if ($min_max == 0) {	
	$foo = $spec_display->plot([\@x_array,\@traceA,\@marker_array]);
	}
else {
	$foo = $spec_display->plot([\@x_array,\@traceA,\@min_traceA,\@max_traceA,\@marker_array]);
	}

my $black = $foo->colorAllocate(0,0,0);
my $red = $foo->colorAllocate(255,0,0);

#-----------
# add text and number formatting features
my $gd_text = GD::Text->new() or die GD::Text::error();

$gd_text->set_font(gdLargeFont);
$gd_text->set_text($title);
my ($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdLargeFont,($x/2)-($w/2),3,$title,$black);

$gd_text->set_font(gdSmallFont);
$gd_text->set_text($annotation1);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdSmallFont,($x/2)-($w/2),20,$annotation1,$black);

$gd_text->set_font(gdSmallFont);
$gd_text->set_text($annotation2);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdSmallFont,($x/2)-($w/2),34,$annotation2,$black);

$gd_text->set_font(gdSmallFont);
$gd_text->set_text($ref_suf);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdSmallFont,19,32,$ref_suf,$black);

if ($num_sweeps > 0) {
	$gd_text->set_font(gdSmallFont);
	$gd_text->set_text('VidAvg:');
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdSmallFont,44,57,'VidAvg:',$black);
	$gd_text->set_font(gdSmallFont);
	$gd_text->set_text($num_sweeps);
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdSmallFont,63-($w/2),70,$num_sweeps,$black);
	}

if ($min_max > 0) {
	$gd_text->set_font(gdSmallFont);
	$gd_text->set_text('Min/Max');
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdSmallFont,44,90,'Min/Max',$black);
	}

my $startstring = "Start: " . $numfmt->format_number($start_freq);
$gd_text->set_font(gdTinyFont);
$gd_text->set_text($startstring);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdTinyFont,50,$y-38,$startstring,$black);

my $cfstring = "Center: " . $numfmt->format_number($center_freq) . " Hz";
$gd_text->set_font(gdSmallFont);
$gd_text->set_text($cfstring);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdSmallFont,($x/2)-($w/2)+($l_margin/2),$y-40,$cfstring,$black);

my $stopstring = "Stop: " . $numfmt->format_number($stop_freq);
$gd_text->set_font(gdTinyFont);
$gd_text->set_text($stopstring);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdTinyFont,440,$y-38,$stopstring,$black);

my $spanstring = "Span: " . $numfmt->format_number(squash($span_val)) .
		" Hz (" . squash($span_val)/10 . " Hz/)";
	;
$gd_text->set_font(gdSmallFont);
$gd_text->set_text($spanstring);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdSmallFont,($x/2)-($w/2)+($l_margin/2),$y-26,
	$spanstring,$black);

$gd_text->set_font(gdTinyFont);
$gd_text->set_text($left_footer);
$foo->string(gdTinyFont,20,$y-10,$left_footer,$black);

$gd_text->set_font(gdTinyFont);
$gd_text->set_text($center_footer);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdTinyFont,($x/2)-($w/2)+($l_margin/2),$y-10,
	$center_footer,$black);

my $timestamp;
if (!$transparent) {
	$timestamp = gmtime() . " (UTC)";
	$gd_text->set_font(gdTinyFont);
	$gd_text->set_text($timestamp);
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdTinyFont,$x-$w-12,$y-10,$timestamp,$black);
	}

binmode PIC;
print PIC $foo->png;
close PIC;

exit 0;
