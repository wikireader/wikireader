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
  ./configure --disable-tcl-binding --disable-php-binding --disable-perl-binding --disable-guile-binding --prefix=/usr
  make clean
  make
  make -C language/python
  modprobe -v -r tnt4882
  make install
  make -C language/python install

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
