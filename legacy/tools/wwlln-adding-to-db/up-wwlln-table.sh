#!/bin/bash

set -e

read -p "Enter mysql root password: " pass

database_name=lightning_detection_system

mysql -u root -p${pass} ${database_name} < wwlln-solutions.sql

echo "Done."
