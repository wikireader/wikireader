#!/usr/bin/env bash

for i in $1/*/spacing/*.gz;
do
    NAME=`dirname $i`/`basename $i .gz`
    zcat $i | sort -u > $NAME
    echo $i;
done
