#!/bin/bash

SRCDIR="test/src/"
RESULTDIR="test/results/"

# Generate output from interpreted tests and compare to previous output
for entry in $SRCDIR*
do
    name=$(b=${entry##*/}; echo ${b%.*})
    bash -c "./build/glomp -i $entry 2>&1 | tee ${RESULTDIR}${name}.txt &>/dev/null"
done
