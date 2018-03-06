#!/bin/bash

systemctl stop dfclient.service
systemctl disable dfclient.service

crontab -u boltek -l | grep -v push_data.sh > /tmp/crontab-backup.txt
crontab -u boltek /tmp/crontab-backup.txt

rm -f ~/.local/bin/dfclient.sh
rm -f ~/.local/bin/experiment_start.sh
rm -f ~/.local/bin/experiment_continue.sh
rm -f ~/.local/bin/push_data.sh
rm -f ~/.local/bin/rsync_dfclient_dir.sh
rm -f ~/.local/bin/dfclient
rm -rf ~/.config/dfclient

rmmod boltek

rm -f /lib/modules/$(uname -r)/kernel/drivers/pci/boltek.ko
rm -f /etc/modules-load.d/boltek.conf
rm -f /etc/systemd/system/dfclient.service


