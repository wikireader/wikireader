#!/usr/bin/env bash

# Small script to create the slave directories
for thread in `seq 1 6`;
do
	for slave in `seq 1 4`;
	do
		mkdir -p zecke-$slave/$thread;
	done;
done;
