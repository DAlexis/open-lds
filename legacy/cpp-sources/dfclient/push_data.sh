#!/bin/bash

set -e

function check_number {
	re='^[0-9]+$'
	if ! [[ $1 =~ $re ]] ; then
	   return 1
	fi
	return 0
}

dfclient_conf=$HOME/.config/dfclient/dfclient.conf

device_id=`grep ^device_id $dfclient_conf | awk -F= '{print $2}'`
host=`grep ^host $dfclient_conf | awk -F= '{print $2}'`
user=`grep ^user $dfclient_conf | awk -F= '{print $2}'`
password=`grep ^password $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`
name=`grep ^name $dfclient_conf | awk -F= '{print $2}'`

sshhost=`grep ^ssh_host $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`
sshuser=`grep ^ssh_user $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`
dbhost=`grep ^db_host $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`
dbuser=`grep ^db_user $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`
dbpassword=`grep ^db_password $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`
dbname=`grep ^db_name $dfclient_conf | awk -F= '{print $2}' | tr -d ' '`

# Get last_id from server with device_id of this device
echo "Receiving last id from server..."
last_id=`ssh -C $sshuser@$sshhost "mysql -u $dbuser -p$dbpassword -e 'SELECT max(id) FROM strikes WHERE device_id=$device_id' $dbname | tail -n 1"`

# If server's database does not contain strikes with this device_id
if [ "$last_id" == "NULL" ] ; then
	last_id="1"
fi

echo $last_id
check_number $last_id

# Get local last id
local_last_id=`mysql -u $user -p$password -e 'SELECT max(id) FROM strikes' $dbname | tail -n 1`

# If local database is empty
if [ "$local_last_id" == "NULL" ] ; then
	local_last_id="0"
fi
check_number $local_last_id

# if last id in local table lesser than that in server's table, we should update local one
if [ $last_id \> $local_last_id ] ; then
	echo "Updating local strike id counter"
	mysql -u $user -p$password -e "INSERT INTO strikes(id) VALUES($last_id)" $dbname
fi

echo "Making local dump..."
dump_fn=`date +%F%H%M%S.sql`

where="id>$last_id"
mysqldump -u $user -p$password --skip-add-lock --lock-tables=false --single-transaction --no-create-db --no-create-info -c -w $where $name strikes > $dump_fn

# import dump to server db
echo "Importing dump to server..."
(ssh -C $sshuser@$sshhost "mysql -u $dbuser -p$dbpassword $dbname") < $dump_fn 

echo "Removing local dump"
rm $dump_fn

date "+%F %H:%M:%S" > /tmp/dfclient-last-push-time.txt

echo "done"
