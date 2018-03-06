#!/bin/bash

set -e

./build-ldf-software.sh $*

./build-driver.sh $*
