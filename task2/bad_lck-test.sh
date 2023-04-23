#!/bin/bash
touch test.txt
echo "test 1337" > test.txt
./main test.txt -s 1 -e &
rm -f test.txt.lck

./main test.txt -s 1 -e & 
sleep 1
echo "1337" > test.txt.lck

rm -f test.txt.lck
rm -f test.txt
