#!/usr/bin/env bash


for job in work/`hostname`/*/wget;
do
    fetch_wget.sh $job &
done


wait


