#!/bin/bash

set -e

echo "WARNING! time_clusters, solutions, strikes, unified_strikes will be DROPPED!"
read -p "Enter mysql root password: " pass

database_name=lightning_detection_system

mysql -u root -p${pass} ${database_name} < drop-tables.sql
mysql -u root -p${pass} ${database_name} < up-unified-table.sql

echo "All operations succeeded."
