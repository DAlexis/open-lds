Lightning detection system software installation guide
=================

The Institute of Applied Physics of the Russian Academy of Sciences
Lightning Physics Laboratory
Aleksey Bulatov, Fedor Kuterin

# Lightning detector
This is a plan how to install one lightning detection unit

## dfclient installation

Make this:

 - Create user `boltek`
 
 - Manually copy `total-dfclient-installer.sh` into boltek user's home dir
 
 - Run it and type passwords when needed

## dfclient configuration

Do not forget to edit this fields in `dfclient.conf`:
```
[Receiver]
hardware     = boltek

[Identification]
device_id     = 3

[Output-provider-mysql]
password      = ldf_test
name          = lightning_detection_system

# This group currently not used by dfclient, only for push_data.sh
[Remote-database]
db_password    = xxxxxx
```

## New device registration
Add corresponding line to `devices` table at server database

## Conky monitor configuration

Run `conky/install-config.sh`, this will install configuration and 
autorun files. Do not forget to edit `.conkyrc` and write proper network
interface name

# Server side

To be continued...
