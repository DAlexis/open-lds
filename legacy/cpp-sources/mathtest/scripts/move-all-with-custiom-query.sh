#!/bin/bash

echo "Moving to local unified_flashes_data base strikes with $1"

./mathtest --source-hostname=ldfdb.lightninglab.ru --source-username=root --source-password=boltek123 \
           --destination-hostname=localhost   --destination-username=root --destination-password=boltek123 \
           --source-table=boltek \
           --custom-query="$1" \
           --write-group-to-server-base \
           --verbose

