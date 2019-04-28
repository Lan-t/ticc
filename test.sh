#!/bin/bash
try() {
    expected=$1
    input=$2

    ./ticc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual=$?

    if [ $actual = $expected ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 7 "7;"
try 34 "20+14;"
try 80 "40 / 2 * 4;"
try 32 "4 * (4 + 4 );"
try 7 "a = 7; a;"
try 72 "a = 9; b = 8; a * b;"

echo OK