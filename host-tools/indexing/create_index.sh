#!/bin/sh

echo "Input file is $1"
cat $1 | LC_ALL=C sort -fu > indexfile.index.sorted
./locate/lbigram < indexfile.index.sorted | sort -nr | perl -ne '/^\s*[0-9]+\s(..)$/ && print $1' > indexfile.index.bigrams
./locate/lcode indexfile.index.bigrams < indexfile.index.sorted > index.locate.db
./locate/lsearcher -f index.locate.db -c index.locate.prefix -n
