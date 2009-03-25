#!/usr/bin/env bash


for job in work/`hostname`/*;
do
    fetch_data.py $job &
done


wait


