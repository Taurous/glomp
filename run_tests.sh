#!/bin/bash

SRCDIR="test/src/"
OUTDIR="test/output/"
RESULTDIR="test/results/"

for entry in $OUTDIR*
do
    name=$(b=${entry##*/}; echo ${b%.*})
    #bash -c "./$entry 2>&1 | tee ${RESULTDIR}${file}.txt"
    diff ${RESULTDIR}${name}.txt ${RESULTDIR}${name}_test.txt &>/dev/null
    if [ $? -gt 0 ]; then
        echo Test failed: $name
        echo expected:
        echo `cat ${RESULTDIR}${name}_test.txt`
        echo got:
        echo `cat ${RESULTDIR}${name}.txt`
    fi
done
