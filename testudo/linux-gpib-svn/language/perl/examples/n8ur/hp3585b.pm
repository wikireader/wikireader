package hp3585b;
#
# John R. Ackermann N8UR   (jra@febo.com)
# Mon Jun  9 09:19:03 2003
#
# hp3585b - Functions for HP 3585B Spectrum Analyzer
#
# Copyright 2003 by John R. Ackermann  N8UR (jra@febo.com)
# Licensed under the GPL version 2 or later; see the file COPYING
# included with this distribution.  I request, but do not require, that
# any modifications that correct bugs or errors, or increase the program's
# functionality, be sent via email to the author at the address above.

use strict;
use warnings;
use diagnostics;
use Exporter;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS);
use n8ur qw(trim squash collapse round parse_value lower_case);

$VERSION     = '0.01';
@ISA         = qw(Exporter);
@EXPORT      = ();
@EXPORT_OK   = qw(make_trace get_settings fix_freq fix_db fix_sweep);

sub make_trace {
# The binary dump starts with two sync bytes, so we throw those away.
# Then we convert 1001 pairs of bytes from 2s-complement to integers, 
# which represent dBV*100 (unless we're doing B-A, in which case they're 
# pure dB*100). The calling routine needs to add 13 to convert from dBV 
# to dBm (50 ohm) if that's required.

	my $j;
	my $rawtrace = shift;
	my @trace = unpack 'n*', substr($rawtrace,2,2002);
	for ($j=0;$j<=1000;$j++) {
		if ($trace[$j] > 32767) {
			$trace[$j] -= 65536;
			}
		$trace[$j] = ($trace[$j]/100);
		}

	return @trace;
}

sub get_settings {
my $command;
my $reading;
my $dev = shift;

# get screen annotation
$command = "D7T4";
LinuxGpib::ibwrt($dev,$command,length($command));
LinuxGpib::ibrd($dev,$reading,174);
# parse the string into one variable for each reading; there's much more
# work to be done later
my ($ref,$top_right,$db_div,$range,$marker,$center,$span,$rbw,$vbw,$sweep) =
	 split(/\n/,$reading);

# get marker freq and amplitude (overrides the previous dump)
$command = "D2T4";
LinuxGpib::ibwrt($dev,$command,length($command));
LinuxGpib::ibrd($dev,$reading,26);
my ($marker_freq,$marker_val) = split(/,/,$reading);
$marker_freq = substr(trim($marker_freq),1);
$marker_val = trim(sprintf("%5.2f",trim($marker_val)));

#---------------
# Now, start string handling.  The annotation data is dumped in a 
# non-parser-friendly way and breaking all the elements out is a lot more
# work than it should be.  Varying field lengths and embedded spaces in
# frequency strings make life interesting.
$ref = trim($ref);
my ($ref_pre,$ref_val,$ref_suf) = split(/\s/,$ref);
if ($ref_suf eq "DBM") {$ref_suf = "dBm"; }
if ($ref_suf eq "DBV") {$ref_suf = "dBV"; }
if ($ref_suf eq "DB") {$ref_suf = "dB"; }

$top_right = trim($top_right);
my $top_right_suf = trim(substr($top_right,length($top_right)-2,2));
my $top_right_pre = trim(substr($top_right,0,index($top_right," ")));
if ($top_right_pre eq "COUNTER") {$top_right_pre = "Counter"; }   
if ($top_right_pre eq "MARKER") {$top_right_pre = "Marker"; }
if ($top_right_pre eq "OFFSET") {$top_right_pre = "Offset"; }   
if ($top_right_pre eq "MANUAL") {$top_right_pre = "Manual"; }   
my $top_right_val = squash(trim(substr(
	$top_right,length($top_right_pre),
	length($top_right)-length($top_right_pre)-length($top_right_suf))));
if ($top_right_suf eq "DBM") {$top_right_suf = "dBm"; }
if ($top_right_suf eq "DBV") {$top_right_suf = "dBV"; }
if ($top_right_suf eq "DB") {$top_right_suf = "dB"; }

$db_div = trim($db_div);
my ($db_div_val,$db_div_suf) = split(/\s/,$db_div);
$db_div_val = trim($db_div_val);
$db_div_suf = trim($db_div_suf);

$range = trim($range);
my ($range_pre,$range_val,$range_suf) = split(/\s/,$range);
$range_pre = trim($range_pre);
$range_val = trim($range_val);
$range_suf = trim($range_suf);
if ($range_suf eq "DBM") {$range_suf = "dBm"; }
if ($range_suf eq "DB") {$range_suf = "dB"; }

$center = trim($center);
my $center_suf = trim(substr($center,length($center)-2,2));
my $center_pre = trim(substr($center,0,index($center," ")));
my $center_val = squash(trim(substr(
	$center,length($center_pre),
	length($center)-length($center_pre)-length($center_suf))));

$span = trim($span);
my $span_suf = trim(substr($span,length($span)-2,2));
my $span_pre = trim(substr($span,0,index($span," ")));
my $span_val = squash(trim(substr(
	$span,length($span_pre),
	length($span)-length($span_pre)-length($span_suf))));

$rbw = trim($rbw);
my ($rbw_pre,$rbw_val,$rbw_suf) = split(/\s/,$rbw);
$rbw_pre = trim($rbw_pre);
$rbw_val = trim($rbw_val);
$rbw_suf = "Hz";

$vbw = trim($vbw);
my ($vbw_pre,$vbw_val,$vbw_suf) = split(/\s/,$vbw);
$vbw_pre = trim($vbw_pre);
$vbw_val = trim($vbw_val);
$vbw_suf = "Hz";

$sweep = trim($sweep);
my ($sweep_pre,$sweep_val,$sweep_suf) = split(/\s/,$sweep);
$sweep_pre = trim($sweep_pre);
$sweep_val = trim($sweep_val);
if (trim($sweep_suf) eq "SEC") {$sweep_suf = "Sec."};
if (trim($sweep_suf) eq "MIN") {$sweep_suf = "Min."};
if (trim($sweep_suf) eq "HR") {$sweep_suf = "Hr."};

# get bottom of scale
my $ref_bottom = $ref_val-($db_div_val*10);

# figure out start, center, stop, and Hz/bin
my $start_freq;
my $center_freq;
my $stop_freq;
my $span_freq;
my $bin_freq;

if (($center_pre eq "CENTER") && ($span_pre eq "SPAN")) {
	$center_freq = $center_val;
	$span_freq = $span_val;
	$start_freq = $center_freq - ($span_freq/2);
	$stop_freq = $center_freq + ($span_freq/2);
	$bin_freq = $span_freq/1000;
	}

if (($center_pre eq "START") && ($span_pre eq "STOP")) {
	$span_freq = $span_val - $center_val;
	$start_freq = $center_freq;
	$stop_freq = $span_freq;
	$center_freq = $start_freq + ($span/2);
	$bin_freq = $span_freq/1000;
	}
return $start_freq,$center_freq,$stop_freq,$bin_freq,$top_right_pre,
	$top_right_val,$top_right_suf,$marker_freq,$marker_val,$rbw_val,
	$vbw_val,$db_div_val,$sweep_val,$sweep_suf,$range_val,$range_suf,
	$ref_val,$ref_suf,$ref_bottom,$span_val;
}

sub fix_freq {
	# input is a string with embedded frequency and suffix
	# strips commas and spaces (but not periods) from frequency
	# returns frequency . suffix in 3585B-compatible format

	my $tmp_val = "";
	my $tmp_suffix = "";

        (undef,$tmp_val,$tmp_suffix) = parse_value(shift);
        $tmp_suffix = lower_case($tmp_suffix);
        if (    ($tmp_suffix ne "hz") &&
                ($tmp_suffix ne "khz") &&
                ($tmp_suffix ne "kz") &&
                ($tmp_suffix ne "mhz") &&
                ($tmp_suffix ne "mz" ) ) {
		return "";
                }

	$tmp_val = squash($tmp_val);
	local $_ = $tmp_val;
	/s/,//gs;
	$tmp_val = $_;

        if ($tmp_suffix eq "hz") { $tmp_suffix = "HZ"};
        if ($tmp_suffix eq "khz") { $tmp_suffix = "KZ"};
        if ($tmp_suffix eq "kz") { $tmp_suffix = "KZ"};
        if ($tmp_suffix eq "mhz") { $tmp_suffix = "MZ"};
        if ($tmp_suffix eq "mz") { $tmp_suffix = "MZ"};
	return $tmp_val . $tmp_suffix;
}

sub fix_db {
	# input is a string with embedded value and suffix
	# strips commas and spaces (but not periods) from value
	# returns db value . suffix in 3585B-compatible format

	my $tmp_val = "";
	my $tmp_suffix = "";

        (undef,$tmp_val,$tmp_suffix) = parse_value(shift);
        $tmp_suffix = lower_case($tmp_suffix);
        if (    ($tmp_suffix ne "db") &&
                ($tmp_suffix ne "dbm") &&
                ($tmp_suffix ne "dbv" ) ) {
		return "";
                }

	$tmp_val = squash($tmp_val);
	local $_ = $tmp_val;
	/s/,//gs;
	$tmp_val = $_;

        if ($tmp_suffix eq "db") { $tmp_suffix = "DB"};
        if ($tmp_suffix eq "dbm") { $tmp_suffix = "DM"};
        if ($tmp_suffix eq "dbv") { $tmp_suffix = "DV"};
	return $tmp_val . $tmp_suffix;
}

sub fix_sweep {
	# input is a string with embedded value and suffix
	# strips commas and spaces (but not periods) from value
	# returns seconds . suffix in 3585B-compatible format

	my $tmp_val = "";
	my $tmp_suffix = "";

        (undef,$tmp_val,$tmp_suffix) = parse_value(shift);
	$tmp_val = squash($tmp_val);
	local $_ = $tmp_val;
	/s/,//gs;
	$tmp_val = $_;

	return $tmp_val . "SC";
}


1;

=pod

=head1 NAME

hp3585b - Functions for HP 3585B Spectrum Analyzer

=head1 SYNOPSIS

    use hp3585b;

    @array = get_settings(@bin_dump)
    ($start_freq,$center_freq,$stop_freq,$bin_freq,$top_right_pre,
	$top_right_val,$top_right_suf,$marker_freq,$marker_val,$rbw_val,
	$vbw_val,$db_div_val,$sweep_val,$sweep_suf,$range_val,$range_suf,
	$ref_val,$ref_suf,$ref_bottom,$span_val) = get_settings(device)

    $freq = fix_freq($raw_freq)   
    $db = fix_freq($raw_db)   
    $sweep_time = fix_sweep($raw_sweep)   

=head1 ABSTRACT

=head1 DESCRIPTION

=head1 FUNCTIONS

=head2 C<new>


=over 4

=item Arg

Argument

=back

=cut

=head1 SEE ALSO

L<My::Module::Tutorial>, L<perlpod|Pod::perlpod>, L<perlpodspec|Pod::perlpodspec>

=head1 LICENSE

This software is released under the same terms as perl itself.
If you don't know what that means visit http://perl.com/

=head1 AUTHOR

Copyright (C) root 2003
All rights Reserved

=cut

