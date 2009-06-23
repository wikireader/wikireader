package LinuxGpib;

use 5.006;
use strict;
use warnings;
use Carp;

require Exporter;
require DynaLoader;
use AutoLoader;

our @ISA = qw(Exporter DynaLoader);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use LinuxGpib ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	ibcac
	ibclr
	ibcmd
	ibconfig
	ibdev
	ibdma
	ibeot
	ibevent
	ibfind
	ibgts
	iblines
	ibloc
	ibonl
	ibpad
	ibrd
	ibrdi
	ibrpp
	ibrsp
	ibrsv
	ibsad
	ibsic
	ibsre
	ibtmo
	ibtrg
	ibwait
	ibwrt
	ibwrti
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(

);
our $VERSION = '0.01';

    use constant EDVR               => 0;
    use constant ECIC               => 1;
    use constant ENOL               => 2;
    use constant EADR               => 3;
    use constant EARG               => 4;
    use constant ESAC               => 5;
    use constant EABO               => 6;
    use constant ENEB               => 7;
    use constant EDMA               => 8;
    use constant EBTO               => 9;
    use constant EOIP               => 10;
    use constant ECAP               => 11;
    use constant EFSO               => 12;
    use constant EOWN               => 13;
    use constant EBUS               => 14;
    use constant ESTB               => 15;
    use constant ESRQ               => 16;
    use constant ETAB               => 20;
    use constant ELCK               => 21;
    use constant TNONE              => 0;
    use constant T10us              => 1;
    use constant T30us              => 2;
    use constant T100us             => 3;
    use constant T300us             => 4;
    use constant T1ms               => 5;
    use constant T3ms               => 6;
    use constant T10ms              => 7;
    use constant T30ms              => 8;
    use constant T100ms             => 9;
    use constant T300ms             => 10;
    use constant T1s                => 11;
    use constant T3s                => 12;
    use constant T10s               => 13;
    use constant T30s               => 14;
    use constant T100s              => 15;
    use constant T300s              => 16;
    use constant T1000s             => 17;


sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.  If a constant is not found then control is passed
    # to the AUTOLOAD in AutoLoader.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "& not defined" if $constname eq 'constant';
    my $val = constant($constname, @_ ? $_[0] : 0);
    if ($! != 0) {
	if ($! =~ /Invalid/ || $!{EINVAL}) {
	    $AutoLoader::AUTOLOAD = $AUTOLOAD;
	    goto &AutoLoader::AUTOLOAD;
	}
	else {
	    croak "Your vendor has not defined LinuxGpib macro $constname";
	}
    }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
	if ($] >= 5.00561) {
	    *$AUTOLOAD = sub () { $val };
	}
	else {
	    *$AUTOLOAD = sub { $val };
	}
    }
    goto &$AUTOLOAD;
}

bootstrap LinuxGpib $VERSION;

# Preloaded methods go here.
sub new {
    my $pkg = shift;
    my $dev = undef;
    my $i;

    if (@_ == 1) {
        $dev = ibfind($_[0]);
    } elsif (@_ == 6) {
        $dev = ibdev(@_);
    } else {
        die("Bad parameter list to LinuxGpib::new($pkg @_)");
    }

    # This has to be tested changed (use status vars)
		# deadlock, if device not found

    bless \$dev, $pkg;
    return $dev;
}
# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

  LinuxGpib - Perl extension for Gpib (uses libgpib from http://linux-gpib.sourceforge.net)

=head1 SYNOPSIS

  use Gpib;

=head1 DESCRIPTION

  This module allows access to libgpib.so from within Perl.

  WARNING: Not all functions have been tested yet!
  USE IT AT YOUR OWN RISK!

=head2 EXPORT

None by default.

=head2 Exportable functions

  int ibcac(int ud, int v)
  int ibclr(int ud)
  int ibcmd(int ud, char *cmd, unsigned long cnt)
  int ibconfig( int ud, int option, int value )
  int ibdev(int minor, int pad, int sad, int timo, int eot, int eos)
  int ibdma( int ud, int v )
  int ibeot(int ud, int v)
  int ibevent(int ud, short *event)
  int ibfind(char *dev)
  int ibgts(int ud, int v)
  int iblines(int ud, unsigned short *buf)
  int ibloc(int ud)
  int ibonl(int ud, int onl)
  int ibpad(int ud, int v)
  int ibrd(int ud, char *rd, unsigned long cnt)
  int ibrdi(int ud, char *rd, unsigned long cnt)
  int ibrpp(int ud, char *ppr)
  int ibrsp(int ud, char *spr)
  int ibrsv(int ud, int v)
  int ibsad(int ud, int v)
  int ibsic(int ud)
  int ibsre(int ud, int v)
  int ibtmo(int ud,int v)
  int ibtrg(int ud)
  int ibwait(int ud, int mask)
  int ibwrt(int ud, char *rd, unsigned long cnt)
  int ibwrti(int ud, char *rd, unsigned long cnt)


=head1 AUTHOR

  Thomas Nisius <tnisius@web.de>

=head1 SEE ALSO



=cut
