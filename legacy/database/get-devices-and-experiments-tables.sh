#!/bin/bash

set -e

server_user=root
server_database=lightning_detection_system

target_user=root
target_database=lightning_detection_system


read -p "Enter mysql server password: " server_password
read -p "Enter mysql server host: " server_host
read -p "Enter mysql server database name: " server_database

read -p "Enter mysql target password: " target_password
read -p "Enter mysql target host: " target_host
read -p "Enter mysql target database name: " target_database

mysqldump -u $server_user -p$server_password -h $server_host --lock-tables=false --single-transaction --no-create-db -c $server_database experiments > /tmp/experiments.sql
mysqldump -u $server_user -p$server_password -h $server_host --lock-tables=false --single-transaction --no-create-db -c $server_database devices > /tmp/devices.sql
echo "Dumps done"

mysql -u $target_user -p$target_password -h $target_host $target_database < /tmp/experiments.sql
mysql -u $target_user -p$target_password -h $target_host $target_database < /tmp/devices.sql
echo "Tables copied"

#rm /tmp/experiments.sql /tmp/devices.sql


