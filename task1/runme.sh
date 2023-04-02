#!/bin/bash
echo -n "generate fileA"; sleep 0.5; echo -n "."; sleep 0.5; echo ".";
python3 gen_fileA.py
echo "build sparce"
make
./main fileA fileB
gzip -k fileA fileB
gzip -cd fileB.gz | ./main fileC
./main fileA fileD -b 100
echo -n "Create stat" ; sleep 0.5; echo -n "."; sleep 0.5; echo ".";
stat fileA fileB fileC fileD fileA.gz fileB.gz > result.txt
echo -n "Delete garbage"; sleep 0.5; echo -n "."; sleep 0.5; echo ".";
rm -f file? file?.gz ./main
