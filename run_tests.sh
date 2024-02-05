#!/bin/bash

RED='\033[4;92m'
RESET='\033[0m'

SRCDIR="test/"
OUTDIR="output/"

$(b=${s##*/}; echo ${b%.*})

for entry in ${SRCDIR}*.glmp
do
    filename=$(b=${entry##*/}; echo ${b%.*})
    #echo -e ${RED}$entry${RESET}
    ./build/glomp -c -o $OUTDIR$filename $entry
done
