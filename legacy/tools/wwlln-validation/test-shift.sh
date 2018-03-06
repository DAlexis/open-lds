#!/bin/bash

LANG=en_US

for i in `seq -1.5 0.1 1.5`; do
echo $i >> report.txt
./validate.py -pboltek123 -t0.1 -s$i | grep vicinity >> report.txt
done

