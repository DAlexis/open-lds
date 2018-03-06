#!/bin/bash

set -e

dest=/lib/modules/$(uname -r)/kernel/drivers/pci
build=cpp-sources/build/driver

echo "   Copying kernel module to $dest"
cp $build/boltek.ko $dest

echo "   Running depmod..."
depmod

echo "   Adding file to /etc/modules-load.d/"
echo "boltek" > /etc/modules-load.d/boltek.conf

echo "   Installing a service"
cp dfclient.service /etc/systemd/system/

echo "   Enabling a service"
systemctl reenable dfclient

