#!/usr/bin/perl
#
# 8569b.pl
# version 0.7 -- 23 June 2003
#
# Plot display of HP 8569B Spectrum Analyzer
# 
# Copyright 2003 by John R. Ackermann  N8UR (jra@febo.com)
# Licensed under the GPL version 2 or later; see the file COPYING
# included with this distribution.  I request, but do not require, that
# any modifications that correct bugs or errors, or increase the program's
# functionality, be sent via email to the author at the address above.
#
# Current status:
# Version 0.7 -- no code changes, but licensed under the GPL now.
#
# Version 0.6 -- this is a stable, working version that I'm going to gut
# pretty soon; I want to rewrite it to match some of the features of the
# hp3585b.pl program.  I also want to add the ability to display not only
# the peak signal, but also the minimum signal and the delta between them.
#
# If you use the -x,-x params to change the image size, the text probably
# won't line up correctly.  Use these at your own risk.


use strict;
use POSIX qw(setsid);
use Getopt::Std;
use Time::HiRes qw(usleep);
use LinuxGpib;
use GD;
use GD::Text;
use GD::Graph::lines;

#----------
# subroutines
sub trim {
    local $_ = shift;
    s/\n/ /sg;		# convert newlines to spaces
    s/\r/ /sg;		# convert carriage returns to spaces
    s/\000/ /sg;	# convert nulls to spaces
    s/^\s+//sg;		# trim leading spaces
    s/\s+$//sg;		# trim trailing spaces
    return $_;
}

sub collapse {
    local $_ = shift;
    s/\s+/ /sg;		# collapse multiple spaces to just one
    return $_;
}

sub round {
        my($number) = shift;
        return int($number +.5 * ($number <=> 0) );
        };


#----------
# usage and option initialization
my $opt_string = 'bdhmr:c:f:t:x:y:';
sub usage() {
print STDERR << "EOF";

usage: $0 [-h] [-b] [-d <device>] [-c 1..4] [-m] [-r] [-t title] -f filename

This program collects data from an HP 8569B spectrum analyzer and generates
a PNG format graphic mimicking the the instrument's display.  Most of the
options affect the output format.

By using the -c and -r options, multiple charts can be created that will
overlay nicely on one another.  The -r option not only makes the image
background transparent, it suppresses the timestamp, so the timestamp of
the underlying non-transparent image will not be blurred.

-h	: this (help) message

-b	: display both traces; default is trace A only

-c 	: use alternate 1 through 4 for trace colors; default is 1 (red,blue)

-d	: device name (from /etc/gpib.conf); default is "hp8569b"

-m	: mark and display values of peak signal

-r	: make output file tRansparent and suppress timestamp; default is no  

-t	: optional title (1 line max)

-x,-y	: output dimension in pixels; default 550x440

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

my $device = "hp8569b";
if ($opt{d}) {
	$device = $opt{d};
	}

my $show_marker = 0;
if ($opt{m}) {
	$show_marker = 1;
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
my $y = 440;
if ($opt{y}) {
	$y = $opt{y};
	}

# set up picfile
open (PIC, ">$picfile") ||
	die "Can't open image file $picfile!\n";

#----------
# initialize instrument
my $dev = LinuxGpib::ibfind($device) ||
	die "Can't open device $device!\n";

# clear instrument
LinuxGpib::ibclr($dev);

sleep 1;

#----------
# get data

# first, get screen annotation
my $command = "CS";
my $reading;
LinuxGpib::ibwrt($dev,$command,length($command));
LinuxGpib::ibrd($dev,$reading,127);

my (undef,undef,$c_freq,$c_freq_unit,undef,$span,$span_unit,undef,undef,
	$rbw,$rbw_unit,undef,$vf,$ref_point,$ref,$ref_value,$db_div,undef,
	undef,$atten,undef,undef,$sweep) = split(/\s/,collapse($reading));

my $annotation1 = "CTR " . trim($c_freq) . " " . trim($c_freq_unit) .
	" | SPAN ". trim($span) .  " ". trim($span_unit) . " | RBW " .
	trim($rbw) . " " . trim($rbw_unit) . " | VF " . trim($vf);
my $annotation2 = trim($ref_point) . " " . trim($ref) . " " .
	trim($ref_value) .  " | " . trim($db_div) .  " dB/ | ATTEN " . 
	trim($atten) . " dB | SWP " . trim($sweep); 

#-------------
# get trace data
$command = "TA";
LinuxGpib::ibwrt($dev,$command,length($command));
LinuxGpib::ibrd($dev,$reading,1924);
my @traceA = split(/,/, trim($reading));

my @traceB;
if ($both) {
	$command = "TB";
	LinuxGpib::ibwrt($dev,$command,length($command));
	LinuxGpib::ibrd($dev,$reading,1924);
	@traceB = split(/,/, trim($reading));
	}

my ($peaka_x, $peaka_y, $peakb_x, $peakb_y);
if ($show_marker) {
	$command = "AP";
	LinuxGpib::ibwrt($dev,$command,length($command));
	LinuxGpib::ibrd($dev,$reading,7);
	($peaka_x,$peaka_y) = split(/,/,trim($reading));
	if ($both) {
		$command = "BP";
		LinuxGpib::ibwrt($dev,$command,length($command));
		LinuxGpib::ibrd($dev,$reading,7);
		($peakb_x,$peakb_y) = split(/,/,trim($reading));
		}
	}
	
#---------------
# plotting routines

# this is just so GD::Graph has x axis labels for us to ignore
my @x_array;
my $i;
for ($i = 0; $i <= 480; $i++) {
	$x_array[$i] = $i;
	}

# these format the axis labels
sub y_format {
my $value = shift;
my $ret;
if ($ref_point eq "MID") {
	$ret = ($value/10) - 40;
	}
else {
	$ret = ($value/10) - 80;
	}
$ret = $ret/(10/trim($db_div));
}

sub x_format {
my $value = shift;
my $ret = (($value/48)-5)*trim($span);
}

# set the trace colors; four sets to choose from
my @cset = [qw(lred lblue)];

if ($colorset == 2) {
	@cset = [qw(lblue lred)];
	}
if ($colorset == 3) {
	@cset =  [qw(green purple)];
	}
if ($colorset == 4) {
	@cset = [qw(purple green)];
	}

# set some dimensions
my $l_margin = 15;
my $r_margin = 15;
my $t_margin = 50;
my $b_margin = 15;

my $spec_display = new GD::Graph::lines($x,$y);

$spec_display->set_title_font(gdTinyFont);

$spec_display->set (
	dclrs => @cset,
	l_margin => $l_margin,
	r_margin => $r_margin,
	t_margin => $t_margin,
	b_margin => $b_margin,
	transparent => $transparent,
	y_max_value => 800,
	y_tick_number => 8,
	y_long_ticks => 1,
	y_number_format => \&y_format,
	x_number_format => \&x_format,
	x_max_value => 480,
	x_tick_number => 10,
	x_long_ticks => 1
	);

my $foo;
if ($both) {
	$foo = $spec_display->plot([\@x_array,\@traceA,\@traceB]);
	}
else
	{
	$foo = $spec_display->plot([\@x_array,\@traceA]);
	}

my $black = $foo->colorAllocate(0,0,0);
my $red = $foo->colorAllocate(255,0,0);

# plot the marker
if ($show_marker) {
	$foo->char(gdSmallFont,
		(($peaka_x/480)*($x-$l_margin-$r_margin))+($l_margin*2)-2,
		$y-(($peaka_y/800)*($y-$t_margin-$b_margin))-($t_margin/2)-8,
		"o",$black);
	if ($both) {
		$foo->char(gdSmallFont,
			(($peakb_x/480)*($x-$l_margin))+$l_margin-1,
			$y-(($peakb_y/800)*($y-$t_margin-$b_margin))-
				($t_margin/2)-8,
			"o",$black);
		}
	}

#-----------
# add text features
my $gd_text = GD::Text->new() or die GD::Text::error();

$gd_text->set_font(gdLargeFont);
$gd_text->set_text($title);
my ($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdLargeFont,275-($w/2),10,$title,$black);

$gd_text->set_font(gdTinyFont);
$gd_text->set_text($annotation1." | ".$annotation2);
($w, $h) = $gd_text->get('width', 'height');
$foo->string(gdTinyFont,285-($w/2),35,$annotation1." | ".$annotation2,$black);

# display marker values
my $marker_x_offset;
my $marker_y_offset;
if ($show_marker) {
	$marker_x_offset = round((($peaka_x-240)/48)*$span);
	my $marker_x_text = "Marker: ". $marker_x_offset .
		" " . substr(trim($span_unit),0,3);
	$gd_text->set_font(gdSmallFont);
	$gd_text->set_text($marker_x_text);
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdSmallFont,40,60,$marker_x_text,$red);

	$marker_y_offset = (($peaka_y - 800)/100)*$db_div;
	my $marker_y_text = "        ". $marker_y_offset . " dB";
	$gd_text->set_font(gdSmallFont);
	$gd_text->set_text($marker_y_text);
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdSmallFont,40,75,$marker_y_text,$red);
	}


my $id = "HP 8569B @ N8UR";
$gd_text->set_font(gdTinyFont);
$gd_text->set_text($id);
$foo->string(gdTinyFont,30,420,$id,$black);

my $timestamp;
if (!$transparent) {
	$timestamp = gmtime() . " (UTC)";
	$gd_text->set_font(gdTinyFont);
	$gd_text->set_text($timestamp);
	($w, $h) = $gd_text->get('width', 'height');
	$foo->string(gdTinyFont,540-$w,420,$timestamp,$black);
	}
binmode PIC;
print PIC $foo->png;
close PIC;

# clear
LinuxGpib::ibclr($dev);
exit 0;
