#!/usr/bin/env python3

import yaml
import mysql.connector
from mysql.connector import errorcode

database_name_production = "lightning_detection_system"
database_name_test = "lightning_detection_system_test"


def add_connection_settings_to_argument_parser(arg_parser):
    """
    Works with argparse
    """
    arg_parser.add_argument("-c", "--connection-settings", dest="connection_settings", default="server.yaml",
                             help="Connection settings .yaml file", metavar="FILENAME.yaml")

    arg_parser.add_argument("-p", "--password", dest="password", default="ldf_test",
                             help="Mysql password", metavar="PASSWORD")
    return arg_parser


def add_connection_settings_to_option_parser(option_parser):
    """
    Works with optparse
    """
    option_parser.add_option("-c", "--connection-settings", dest="connection_settings", default="server.yaml",
                             help="Connection settings .yaml file", metavar="FILENAME.yaml")

    option_parser.add_option("-p", "--password", dest="password", default="ldf_test",
                             help="Mysql password", metavar="PASSWORD")
    return option_parser


def setup_connection_settings(settings, filename, password=None, db='test'):
    with open(filename, 'r') as f:
        conn_config = yaml.load(f.read())

    if 'username' not in conn_config['connection']:
        raise ValueError("Username not specified in config file")
    
    if 'hostname' not in conn_config['connection']:
        conn_config['connection']['hostname'] = 'localhost'
    
    if password is not None:
        conn_config['connection']['password'] = password
    
    if db == 'test':
        conn_config['connection']['database'] = database_name_test
    elif db == 'production':
        conn_config['connection']['database'] = database_name_production
    else:
        raise ValueError("db should be 'test' or 'production'!")

    settings['connection'] = conn_config['connection']
    
    return settings


def create_connection(username, password, hostname, database):
    mysql_connection = mysql.connector.connect(
        user=username,
        password=password,
        host=hostname,
        database=database)
    return mysql_connection, mysql_connection.cursor(buffered=True)


def create_connection_and_cursor(settings):
    return create_connection(
        username=settings['connection']['username'],
        password=settings['connection']['password'],
        hostname=settings['connection']['hostname'],
        database=settings['connection']['database'])
