#!/bin/bash

./mathtest --source-hostname=ldfdb.lightninglab.ru --source-username=root --source-password=boltek123 \
           --destination-hostname=localhost   --destination-username=root --destination-password=boltek123 \
           --source-table=unified \
           --first=$1 \
           --output-field=BE,BN,Dia \
           --direction \
           --verbose
