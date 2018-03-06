#!/bin/bash

set -e

targetUser=boltek
targetBinDir=~/.local/bin
targetModDir=~/.local/modules
targetConfigDir=~/.config/dfclient/
targetLogDir=~/dfclient
mysqlPassword=123qweQWE

mkdir -p $targetBinDir
mkdir -p $targetModDir
mkdir -p $targetConfigDir
mkdir -p $targetLogDir

echo " + Building driver"
(cd cpp-sources && ./build-driver.sh)

echo " + Building software"
(cd cpp-sources && ./build-ldf-software.sh release)

echo " + Copying driver to $targetModDir"
cp cpp-sources/build/driver/boltek.ko $targetModDir
sudo cp cpp-sources/driver/rules/77-boltek.rules /etc/udev/rules.d/

echo " + Copying executable to $targetBinDir"
cp cpp-sources/build/release/dfclient/dfclient $targetBinDir
cp cpp-sources/build/release/dfclient/*.sh $targetBinDir

echo " + Copying config to $targetConfigDir"
cp cpp-sources/build/release/dfclient/*.conf $targetConfigDir

if [ ! -f ~/.selected_editor ] ; then
    select-editor
fi

source ~/.selected_editor

echo " + Creating database user"
( cd database 
  ./updb.py -E -P $mysqlPassword || true
  ./updb.py -B -p $mysqlPassword --production --dfclient-password=$mysqlPassword
  )

echo " + Enabling dfclient autorun"
sudo ./dfclient-autorun-setup.sh

echo " + Now edit configuration files manually"
$SELECTED_EDITOR $targetConfigDir/dfclient.conf
