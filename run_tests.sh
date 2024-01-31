#!/bin/bash

for entry in test_src/*.glmp
do
    echo $entry
    ./build/glomp -i $entry
    echo
done
