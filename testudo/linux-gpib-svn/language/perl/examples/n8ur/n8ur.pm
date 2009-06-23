package n8ur;

# John R. Ackermann N8UR   (jra@febo.com)
# Mon Jun  9 09:19:03 2003
#
# n8ur - my useful functions
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

$VERSION     = '0.01';
@ISA         = qw(Exporter);
@EXPORT      = ();
@EXPORT_OK   = qw(trim collapse squash lower_case upper_case
		 round parse_value is_number);

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

sub squash {
    local $_ = shift;
    s/\s//sg;		# remove all spaces
    return $_;
}

sub lower_case {
    local $_ = shift;
    $_ =~ tr [A-Z] [a-z];
    return $_;
}

sub upper_case {
    local $_ = shift;
    $_ =~ tr [a-z] [A-Z];
    return $_;
}

sub round {
	my($places) = shift;
        my($number) = shift;
	my($rounded);
	if ($number < 0) {
		$rounded = int(($number*10**$places) -.5
			* ($number <=> 0) )/10**$places;
	}
	else {
		$rounded = int(($number*10**$places) +.5
			* ($number <=> 0) )/10**$places;
	}

	return $rounded
        };

sub parse_value {
	# splits input into alpha prefix, numeric value, and alpha suffix
	# first split is when a digit, or "+", "-", or "." is encountered
	# second split is at first alpha after the number
	my($val) = shift;
	my $prefix = "";
	my $value = "";
	my $suffix = "";
	my $j = 0;
	my $end = 0;

	# get rid of any embedded spaces
	$val = squash($val);

	until ( (substr($val,$j,1) =~ /[\d+-\.]/) || ($j == length($val)) ) {
		$prefix .= substr($val,$j,1);
		$j++;
		$end = $j;
		}

	if ($end > 1) {
		$val = substr($val,$end);
		}

	$j = 0;
	$end = 0;
	until ( (substr($val,$j,1) =~ /[a-z]/i) || ($j == length($val)) ) {
		$j++;
		$end = $j;
		}

	$value = substr($val,0,$end);
	$suffix = substr($val,$end);

	return $prefix,$value,$suffix;
}

sub is_number {
	# returns true if input is a decimal number
    	local $_ = shift;
	if ( /^[+-]?(?:\d+(?:\.\d*)?|\.\d+)$/ ) {
		return 1;
		}
	else {
		return 0;
		}
}

1;

=pod

=head1 NAME

hp3585b - Functions for HP 3585B Spectrum Analyzer

=head1 SYNOPSIS

	$string = trim($input)
	$string = collapse($input)
	$string = squash($input)
	$string = lower_case($input)
	$string = upper_case($input)
	$num = round($num_places,$input)
	($prefix,$value,$suffix) = parse_value($string)
	$boolean = is_number($input)

=head1 ABSTRACT

=head1 DESCRIPTION

=head1 FUNCTIONS 


=back

=cut

=head1 SEE ALSO


=head1 LICENSE
 Copyright 2003 by John R. Ackermann  N8UR (jra@febo.com)

 This program may be copied, modified, distributed and used for 
 any legal purpose provided that (a) the copyright notice above as well
 as these terms are retained on all copies; (b) any modifications that 
 correct bugs or errors, or increase the program's functionality, are 
 sent via email to the author at the address above; and (c) such 
 modifications are made subject to these license terms.

=head1 AUTHOR
John Ackermann   N8UR (jra@febo.com

Copyright (C) 2003
All rights Reserved
=cut
