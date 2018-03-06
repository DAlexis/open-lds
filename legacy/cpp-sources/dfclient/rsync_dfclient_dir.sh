#!/bin/bash

set -e

rsync -zavP $HOME/dfclient/ geo-provider@diogen.lightninglab.ru:/mnt/data/observations/lds/`hostname`

date "+%F %H:%M:%S" > /tmp/rsync-dfclient-dir-last-time.txt

