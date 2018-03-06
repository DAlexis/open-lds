#!/bin/bash

set -e

if [[ $1 = "production" ]]
then
    database_name=lightning_detection_system
else
    database_name=lightning_detection_system_test
fi

echo Database: $database_name

read -p "Enter mysql root password: " pass

echo "Do you wish to set root's password?"
select yn in "Yes" "No"
do
    case $yn in
        Yes ) sudo mysqladmin -u root password ${pass}
              break;;
        No ) break;;
    esac
done

echo "Do you wish to create database?"
select yn in "Yes" "No"
do
    case $yn in
        Yes ) echo "CREATE DATABASE lightning_detection_system;" | mysql -u root -p${pass}
              break;;
        No ) break;;
    esac
done


echo "What do you want to drop (if exist)?"
select to_drop in "base" "working_tables" "nothing"
do
    case $to_drop in
        base ) mysql -u root -p${pass} < drop-database.sql
            break;;
        working_tables ) mysql -u root -p${pass} ${database_name} < drop-tables.sql
            break;;
        nothing ) break;;
    esac
done

echo "Create device and experiments tables?"
select yn in "yes" "no"
do
    case $yn in
        yes ) mysql -u root -p${pass} ${database_name} < up-dev-exp-tables.sql
            break;;
        no ) break;;
    esac
done

echo "Create boltek raw data table?"
select yn in "client_version" "server_version" "no"
do
    case $yn in
        client_version ) mysql -u root -p${pass} ${database_name} < up-boltek-raw-client.sql
            break;;
        server_version ) mysql -u root -p${pass} ${database_name} < up-boltek-raw-server.sql
            break;;
        no ) break;;
    esac
done

echo "Create tables for strikes processing?"
select yn in "yes" "no"
do
    case $yn in
        yes ) mysql -u root -p${pass} ${database_name} < up-unified-table.sql
            break;;
        no ) break;;
    esac
done

echo "All operations succeeded."
