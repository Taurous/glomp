#!/bin/bash

SRCDIR="test/src/"
OUTDIR="test/output/"
RESULTDIR="test/results/"

# Compile tests
for entry in ${SRCDIR}*.glmp
do
    filename=$(b=${entry##*/}; echo ${b%.*})
    ./build/glomp -c -o $OUTDIR$filename $entry
done

# Generate output from compiled tests and compare to previous output
for entry in $OUTDIR*
do
    name=$(b=${entry##*/}; echo ${b%.*})
    bash -c "./$entry 2>&1 | tee ${RESULTDIR}${name}_com.txt &>/dev/null"
    diff ${RESULTDIR}${name}.txt ${RESULTDIR}${name}_com.txt &>/dev/null
    if [ $? -gt 0 ]; then
        echo Compiler test failed: $name
        echo expected:
        echo `cat ${RESULTDIR}${name}.txt`
        echo got:
        echo `cat ${RESULTDIR}${name}_com.txt`
    fi
done

# Generate output from interpreted tests and compare to previous output
for entry in $SRCDIR*
do
    name=$(b=${entry##*/}; echo ${b%.*})
    bash -c "./build/glomp -i $entry 2>&1 | tee ${RESULTDIR}${name}_int.txt &>/dev/null"
    diff ${RESULTDIR}${name}.txt ${RESULTDIR}${name}_int.txt &>/dev/null
    if [ $? -gt 0 ]; then
        echo Interpreter test failed: $name
        echo expected:
        echo `cat ${RESULTDIR}${name}.txt`
        echo got:
        echo `cat ${RESULTDIR}${name}_int.txt`
    fi
done
