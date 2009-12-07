#!/bin/bash

xml="xml-file-samples/license.xml xml-file-samples/terms.xml enwiki-pages-articles.xml"


ERROR()
{
  echo error: $*
  exit 1
}

USAGE()
{
  [ -z "$1" ] || echo error: $*
  echo usage: $(basename "$0") '<options>' command
  echo '       --help         -h         this message'
  echo '       --verbose      -v         more messages'
  echo '       --get-index=n  -g <n>     where to rsync the pickles from [1 => render1]'
  echo '       --host=name    -o <name>  name of the host [render]'
  echo '       --index-only   -i         only do the index'
  echo '       --no-run       -n         do not run final make'
  echo '       --sequential   -s         run rensering in series'
  echo '       --clear        -c         clear work and dest dirs'
  echo '       --work=dir     -w <dir>   workdir [work]'
  echo '       --dest=dir     -d <dir>   destdir [image]'
  exit 1
}


verbose=no
host=render
index=
clear=no
work=work
dest=image
run=yes
seq=no
IndexOnly=no
debug=

args=$(getopt -o hvg:o:p:inscw:d: --long=help,verbose,get-index:,host:,index-only,no-run,sequential,clear,work:,dest:,debug -- "$@") ||exit 1
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

    -i|--index-only)
      IndexOnly=yes
      shift
      ;;

    -n|--no-run)
      run=no
      shift
      ;;

    -s|--sequential)
      seq=yes
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

eval ${debug} "mkdir -p '${work}'"
eval ${debug} "mkdir -p '${dest}'"

case "${clear}" in
  [yY]|[yY][eE][sS])
    eval ${debug} "time make clean-index DESTDIR='${dest}' WORKDIR='${work}' XML_FILES='${xml}'"
    eval ${debug} "time make '${farm}-clean' DESTDIR='${dest}' WORKDIR='${work}' XML_FILES='${xml}'"
    eval ${debug} "rm -f '${work}'/* '${dest}'/*"
    ;;
esac

# update
git pull --rebase

# copy the index from another machine
if [ -n "${index}" ]
then
  list='articles.db offsets.db counts.text'
  items=
  for i in ${list}
  do
    items="${items} ${host}${index}:samo/${work}/${i}"
  done
  eval ${debug} "rsync -avHx --progress ${items} '${work}'/"
  eval ${debug} "touch stamp-r-index"
fi

# run the build
case "${run}" in
  [yY]|[yY][eE][sS])

    eval ${debug} "time make 'stamp-r-index' DESTDIR='${dest}' WORKDIR='${work}' XML_FILES='${xml}'"

    case "${IndexOnly}" in
      [yY]|[yY][eE][sS])
        ;;
      *)
        case "${seq}" in
          [yY]|[yY][eE][sS])
            eval ${debug} "time make -j3 '${farm}-parse' DESTDIR='${dest}' WORKDIR='${work}' XML_FILES='${xml}'"
            eval ${debug} "time make '${farm}-render' DESTDIR='${dest}' WORKDIR='${work}' XML_FILES='${xml}'"
            ;;
          *)
            eval ${debug} "time make -j3 '${farm}' DESTDIR='${dest}' WORKDIR='${work}' XML_FILES='${xml}'"
            ;;
        esac
    esac
esac
