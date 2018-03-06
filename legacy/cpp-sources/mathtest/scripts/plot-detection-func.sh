#!/bin/bash

./mathtest --config=df-math.conf \
           --source-password=boltek123 \
           --read-cluster=$1 \
           --detection-function-graph \
           --graph-margins=2 \
           --solve \
           --verbose

