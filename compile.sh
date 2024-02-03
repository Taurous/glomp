#!/bin/bash

OUTPATH="./output/"
FILE=${OUTPATH}${1}

nasm -felf64 ${FILE}.asm -o ${FILE}.o && ld ${FILE}.o -o ${FILE} && ./$FILE
