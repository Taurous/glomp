#!/bin/bash

RED='\033[4;92m'
RESET='\033[0m'

for entry in test_src/*.glmp
do
    echo -e ${RED}$entry${RESET}
    ./build/glomp -i $entry
    echo
done
