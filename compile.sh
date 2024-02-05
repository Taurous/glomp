#!/bin/bash

FILE=$1
OUTPUT="output/"

./build/glomp -c -o $OUTPUT$FILE test_src/${FILE}.glmp
