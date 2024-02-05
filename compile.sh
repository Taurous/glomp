#!/bin/bash

FILE=$1
INPUTDIR="test/"
OUTPUTDIR="output/"

./build/glomp -c -o $OUTPUT$FILE ${INPUTDIR}${FILE}.glmp
