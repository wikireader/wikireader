#!/usr/bin/env bash

echo "Going to directory $1"
cd $1
for file in *.wget;
do
    echo $file
    wget -O /dev/null -i $file
    touch "$file.complete"
done 
