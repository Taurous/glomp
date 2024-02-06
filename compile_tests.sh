#!/bin/bash

SRCDIR="test/src/"
OUTDIR="test/output/"
RESULTDIR="test/results/"

for entry in ${SRCDIR}*.glmp
do
    filename=$(b=${entry##*/}; echo ${b%.*})
    ./build/glomp -c -o $OUTDIR$filename $entry
    if [ $? == 0 ]; then
        bash -c "$OUTDIR$filename 2>&1 | tee ${RESULTDIR}${filename}.txt &>/dev/null"
    fi
done
