#!/bin/sh
# configure and build the gpib drivers

main()
{
  local dir
  dir=linux-gpib-svn
  if ! cd "${dir}"
  then
    echo cannot change to ${dir}
    exit 1
  fi
  [ -e Makefile ] && make clean
  autoreconf --verbose --install --symlink
  ./configure --disable-tcl-binding --disable-php-binding --disable-perl-binding --disable-guile-binding
  make clean
  make
  make -C language/python
  modprobe -v -r tnt4882
  make install
  make -C language/python install

  # update library cache
  ldconfig -v

  echo loading the updated driver
  modprobe -v tnt4882

  echo cleaning up
  make clean
  cd ..

  echo initialising the driver
  gpib_config

  echo installing scripts in init.d and rcX.d
  install -o root -g root -m 750 init-scripts/gpib /etc/init.d/
  for d in 1
  do
    rm -f "/etc/rc${d}.d/K01gpib"
    ln -s ../init.d.gpib "/etc/rc${d}.d/K01gpib"
  done
  for d in 2 3 4 5
  do
    rm -f "/etc/rc${d}.d/S99gpib"
    ln -s ../init.d.gpib "/etc/rc${d}.d/S99gpib"
  done

  echo running the startup script
  /etc/init.d/gpib stop
  sleep 1
  /etc/init.d/gpib start

  echo Add the following to your /etc/sudoers
  echo '----------8<----------8<----------8<---------'
  cat init-scripts/sudoers.add
  echo '----------8<----------8<----------8<---------'
  echo 'Now run: sudo visudo'
  echo '     or: sudo EDITOR=jove visudo'
  echo and paste the above text into file

  # check that the gpib module is usable
  if ! python -m Gpib
  then
    echo '** ERROR:'
    echo '**   The python GPIB.py module is not accessible'
    echo '**   Maybe there are multiple versions of python'
    echo '**   and the wrong one was selected by this install script'
    echo '** OR:'
    echo '**   python development tools were not installed, try:'
    echo '**     sudo apt-get python-dev'
    echo '**   and re-run this script'
  fi
}


if [ -z "${SUDO_UID}" -o -z "${SUDO_GID}" ]
then
  echo re-run with sudo
  sudo -K
  exec sudo "$0" "$@"
  exit 1
fi

if [ -n "${SUDO_UID}" -a -n "${SUDO_GID}" ]
then
  main "$@"
else
  echo sudo failed
fi
