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

try 0 0
try 42 42
try 39 30+9
try 20 94-74
try 10 5+10-5

echo OK