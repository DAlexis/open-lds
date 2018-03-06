#!/bin/bash

target=~/strikesprocd

mkdir -p $target
cp ../cpp-sources/build/release/strikesprocd/df-math.conf $target
cp ../cpp-sources/build/release/strikesprocd/strike-proc-d.conf $target
cp ../cpp-sources/build/release/strikesstatd/strikesstatd.conf $target

