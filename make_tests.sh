#!/bin/bash

RESULTSDIR="test/results/"

for file in ${RESULTSDIR}*.txt
do
    name=$(b=${file##*/}; echo ${b%.*})
    cp $file ${RESULTSDIR}${name}_test.txt
done
