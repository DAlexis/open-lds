#!/bin/bash
set -e

echo -e "\n\033[1;33m[:||| Building driver |||:]\033[0m"

driver_source_dir=./driver/src
build_dir=./build/driver

mkdir -p $build_dir

make -C $driver_source_dir
mv $driver_source_dir/boltek.ko $build_dir
make -C $driver_source_dir clean
