#!/bin/bash

(cd cpp-sources && ./build-ldf-software.sh release)
(cd installers && ./install-processing-deamon.sh)
