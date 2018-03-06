#!/bin/bash

target=~/strikesprocd

mkdir -p $target
cp ../cpp-sources/build/release/strikesprocd/strikes-processing-deamon $target
cp ../cpp-sources/build/release/strikesstatd/strikes-statistics-collector $target
