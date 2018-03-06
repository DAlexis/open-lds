#!/bin/bash
if [ $# -ne 1 ] ; then
    echo "usage: experiment_continue.sh experiment-id"
    exit 1
fi
id=$1
#
n=`echo 'select count(*) from experiments where id='$id | mysql -u root -p123qwe lightning_detection_system -B  | tail -n 1`
if [ $n -ne 1 ] ; then
    echo "No that experiment_id. Get another."
    exit 2
fi
#
echo $id
sed -i -e 's/^experiment_id\s*=\s*[0-9]\+/experiment_id = '$id'/' default.conf
../build/debug/dfclient
