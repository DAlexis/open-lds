#!/bin/bash

echo "Moving to local unified_flashes_data base id's from $1 to $2..."

for i in $(seq $1 $2)
do
    ./mathtest --source-hostname=ldfdb.lightninglab.ru --source-username=root --source-password=boltek123 \
               --destination-hostname=localhost   --destination-username=root --destination-password=boltek123 \
               --source=boltek \
               --first=$i \
               --write-skrike-to-server-base
done
