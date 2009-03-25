#!/usr/bin/env bash


for job in work/`hostname`/*;
do
    fetch_wget.sh $job &
done


wait


