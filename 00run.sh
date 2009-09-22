#!/bin/sh

xml="xml-file-samples/license.xml xml-file-samples/terms.xml enwiki-20090909-pages-articles.xml"

ERROR()
{
  echo error: $*
  exit 1
}

[ -z "$1" ] && ERROR missing argument

set -x
time make "farm$1$2" DESTDIR=image WORKDIR=work XML_FILES="${xml}"
