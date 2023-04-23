#!/bin/bash
make
echo "run bad lck-file tests"
chmod +x bad_lck-test.sh
./bad_lck-test.sh

echo "run stat tests" 
bash stat_test.sh
rm -f ./main
