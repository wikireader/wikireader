#!/bin/sh

xml="xml-file-samples/license.xml xml-file-samples/terms.xml enwiki-20090909-pages-articles.xml"


ERROR()
{
  echo error: $*
  exit 1
}

USAGE()
{
  [ -z "$1" ] || echo error: $*
  echo usage: $(basename "$0") '<options>' command
  echo '       --help         this message'
  echo '       --verbose      more messages'
  echo '       --get-index=n  where to rsync the pickles from [1 => render1]'
  echo '       --host=name    name of the host [render]'
  echo '       --no-run       do not run fuinal make'
  echo '       --clear        clear work and dest'
  echo '       --work=name    workdir [work]'
  echo '       --dest=name    destdir [image]'
  exit 1
}


verbose=no
host=render
index=
clear=no
work=work
dest=image
run=yes
debug=

args=$(getopt -o hvg:o:p:ncw:d: --long=help,verbose,get-index:,host:,no-run,clear,work:,dest:,debug -- "$@") ||exit 1
# replace the arguments with the parsed values
eval set -- "${args}"

while :
do
  case "$1" in
    -v|--verbose)
      verbose=yes
      shift
      ;;

    -g|--get-index)
      index=$2
      shift 2
      ;;

    -o|--host)
      host=$2
      shift 2
      ;;

    -n|--no-run)
      run=no
      shift
      ;;

    -c|--clear)
      clear=yes
      shift
      ;;

    -w|--work)
      work=$2
      shift 2
      ;;

    -d|--dest)
      dest=$2
      shift 2
      ;;

    --debug)
      debug=echo
      shift
      ;;

    --)
      shift
      break
      ;;

    -h|--help)
      USAGE
      ;;

    *)
      USAGE invalid option: $1
      ;;
  esac
done

this_host=$(hostname --short)
this_id=${this_host##${host}}
[ X"${this_id}" = X"${this_host}" ] && this_id=0

farm="farm${this_id}"

${debug} mkdir -p "${work}"
${debug} mkdir -p "${dest}"

case "${clear}" in
  [yY]|[yY][eE][sS])
    ${debug} time make clean-index DESTDIR="${dest}" WORKDIR="${work}" XML_FILES="${xml}"
    ${debug} time make "${farm}-clean" DESTDIR="${dest}" WORKDIR="${work}" XML_FILES="${xml}"
    ${debug} rm -f "${work}"/* "${dest}"/*
    ;;
esac


# copy the index from another machine
if [ -n "${index}" ]
then
  list='articles.pickle offsets.pickle counts.text'
  items=
  for i in ${list}
  do
    items="${items} ${host}${index}:samo/${work}/${i}"
  done
  ${debug} rsync -avHx --progress ${items} ${work}/
  ${debug} touch stamp-r-index
fi

case "${run}" in
  [yY]|[yY][eE][sS])
    ${debug} time make "stamp-r-index" DESTDIR="${dest}" WORKDIR="${work}" XML_FILES="${xml}"
    ${debug} time make -j3 "${farm}-parse" DESTDIR="${dest}" WORKDIR="${work}" XML_FILES="${xml}"
    ${debug} time make "${farm}-render" DESTDIR="${dest}" WORKDIR="${work}" XML_FILES="${xml}"
    ;;
esac
