#!/usr/bin/perl
#
# hp3585b-control.pl
# version 0.8 -- 23 June 2003
#
# Basic control of HP 3585B Spectrum Analyzer
# 
# Copyright 2003 by John R. Ackermann  N8UR (jra@febo.com)
# Licensed under the GPL version 2 or later; see the file COPYING 
# included with this distribution.  I request, but do not require, that 
# any modifications that correct bugs or errors, or increase the program's
# functionality, be sent via email to the author at the address above.
#
# Current status:
#
# Version 0.8 -- No code changes, but now licensed under the GPL.
#
# Version 0.7 -- basic functionality seems to work OK, with (very) modest
# input validation.

#----------

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
use n8ur qw(trim squash parse_value);
use hp3585b qw(get_settings fix_freq fix_db fix_sweep);

my $device = "hp3585b";
my $j;
my $command;
my $reading;
my $counter = 1;
my $gpib_status;
my $tmp_val;
my $tmp_suffix;


#----------
# usage and option initialization
my $opt_string = 'hpc:d:s:l:a:r:v:t:b:';
sub usage() {
print STDERR << "EOF";

usage: $0 [-p] [-c center freq] [-s span] [-l ref level] [-a range] 
	[-r rbw] [-v vbw] [t sweep time] [-b db/div] [-h help] [-d device]

This program provides basic control of an HP 8569B spectrum analyzer and
displays current settings.

Use value suffixes of Hz, kHz, MHz, dB, dBm, dBv as appropriate.
If not specified, params are unchanged.  Default device is "hp3585b".

"-p" does an instrument preset

EOF
}

#----------------------
getopts( "$opt_string", \my %opt ) or usage() and exit;
usage() and exit if $opt{h};

my $preset = 0;
if ($opt{p}) {
	$preset = 1;
	}

if ($opt{d}) {
	$device = $opt{d};
	}

my $range = "";
if ($opt{a}) {
	my $tmp = trim($opt{a});
	if ($tmp eq "-25") {$range = "AR0R01" };
	if ($tmp eq "-20") {$range = "AR0R02" };
	if ($tmp eq "-15") {$range = "AR0R03" };
	if ($tmp eq "-10") {$range = "AR0R04" };
	if (($tmp eq "-5") || ($tmp eq "-05")) {$range = "AR0R05" };
	if ($tmp eq "0")   {$range = "AR0R06" };
	if (($tmp eq "5") || ($tmp eq "05")) {$range = "AR0R07" };
	if ($tmp eq "10") {$range = "AR0R08" };
	if ($tmp eq "15") {$range = "AR0R09" };
	if ($tmp eq "20") {$range = "AR0R10" };
	if ($tmp eq "25") {$range = "AR0R11" };
	if ($tmp eq "30") {$range = "AR0R12" };
	}

my $db_div = "";
if ($opt{b}) {
	$tmp_val = fix_db($opt{b});
	if ( 	($tmp_val ne "10") &&
		($tmp_val ne "5") &&
		($tmp_val ne "2") &&
		($tmp_val ne "1") ) { die "Invalid dB/div value!"}
	else {
		$db_div = "DD" . $tmp_val . "DB"; 
		}
	}

my $cf = "";
if ($opt{c}) {
	$tmp_val = fix_freq($opt{c});
	if ($tmp_val eq "") { die "Invalid Center Frequency value!"}
	else { $cf = "CF" . $tmp_val; }
	}

my $fs = "";
if ($opt{s}) {
	$tmp_val = fix_freq($opt{s});
	if ($tmp_val eq "") { die "Invalid Span value!"}
	else { $fs = "FS" . $tmp_val; }
}

my $rbw = "";
if ($opt{r}) {
	$tmp_val = fix_freq($opt{r});
	if ($tmp_val eq "") { die "Invalid RBW value!"}
	else { $rbw = "RB" . $tmp_val; }
}

my $vbw = "";
if ($opt{v}) {
	$tmp_val = fix_freq($opt{v});
	if ($tmp_val eq "") { die "Invalid RBW value!"}
	else { $rbw = "VB" . $tmp_val; }
}

my $rl = "";
if ($opt{l}) {
	$tmp_val = fix_db($opt{l});
	if ($tmp_val eq "") { die "Invalid ref level value!"}
	else { $rbw = "RL" . $tmp_val; }
	}

my $t = "";
if ($opt{t}) {
	$tmp_val = fix_sweep($opt{t});
	if ($tmp_val eq "") { die "Invalid sweep time value!"}
	else { $rbw = "ST" . $tmp_val; }
	}

#----------
# initialize instrument
my $dev = LinuxGpib::ibfind($device) ||
	die "Can't open device $device!\n";

#----------
# Send command
if ($preset) {
	$command = "PO0";
	}
else {
	$command = "AC0" . $cf . $fs . $rbw . $rl . $range . $db_div . "AC1";
	}
LinuxGpib::ibwrt($dev,$command,length($command));
sleep 3;

#----------
# get instrument state
my ($start_freq,$center_freq,$stop_freq,$bin_freq,$top_right_pre,
        $top_right_val,$top_right_suf,$marker_freq,$marker_val,$rbw_val,
        $vbw_val,$db_div_val,$sweep_val,$sweep_suf,$range_val,$range_suf,
        $ref_val,$ref_suf,$ref_bottom,$span_val) = get_settings($dev);

#----------
# create annotation strings for plotting
my $numfmt = new Number::Format(-thousands_sep => ' ', -decimal_point => '.');
my $annotation1= $top_right_pre . ": " .
	$numfmt->format_number($top_right_val) . " Hz, " .
	$marker_val . " " . $ref_suf;
my $annotation2=
	"RBW: " . $rbw_val . " Hz  VBW: " . $vbw_val . " Hz  Scale: " .
	 $db_div_val .  " dB/  Sweep: " . $sweep_val . " Sec.  Range: " .
	 $range_val . " " . $range_suf;

print "\n";
print "Center: " . $numfmt->format_number($center_freq) .
	" Hz Span: " . $numfmt->format_number(squash($span_val)) .
	" Hz (" . squash($span_val)/10 . " Hz/)\n";
print "Start: " . $numfmt->format_number($start_freq) . " Hz Stop: " .
	$numfmt->format_number($stop_freq) . " Hz\n\n";
print $annotation1,"\n\n";
print "Ref. Level: " . $ref_val . $ref_suf . "\n";
print $annotation2,"\n\n";
