#!/bin/bash
if [ $# -ne 1 ] ; then
    echo "usage: experiment_start.sh 'experiment description'"
    exit 1
fi
description=$1
id=`echo 'insert into experiments(description) values("'$description'"); select LAST_INSERT_ID();' | mysql -u root -p123qwe lightning_detection_system -B  | tail -n 1`
echo $id
sed -i -e 's/^experiment_id\s*=\s*[0-9]\+/experiment_id = '$id' ; '$description'/ ' default.conf
../build/debug/dfclient
