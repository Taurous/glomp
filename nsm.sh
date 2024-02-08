#!/bin/bash

FILE=$1

nasm -felf64 -Fdwarf ${FILE}.asm -o ${FILE}.o && ld ${FILE}.o -o ${FILE} && rm ${FILE}.o && ./$FILE
