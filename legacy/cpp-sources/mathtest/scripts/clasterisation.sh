#!/bin/bash

./mathtest --source-password=boltek123 \
           --destination-password=boltek123 \
           --source-table=unified \
           --custom-query="(id >= $1) AND (id <= $2)" \
           --clasterisation \
           --verbose \
           $*
