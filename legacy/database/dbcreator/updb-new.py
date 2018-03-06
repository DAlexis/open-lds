#!/usr/bin/env python3


import mysqlutils
from tablesCreator import *

from optparse import OptionParser
import sys
import yaml

cmd_line_parser = OptionParser()

cmd_line_parser.add_option("-u", "--user", dest="root_name", default="admin",
                           help="Mysql user (use instead of root to create tables and users)", metavar="USERNAME")

cmd_line_parser.add_option("-p", "--password", dest="root_password",
                           help="Mysql root password", metavar="PASSWORD")

cmd_line_parser.add_option("-H", "--host", dest="hostname", default="localhost",
                         help="Database hostname", metavar="HOSTNAME")

cmd_line_parser.add_option("-t", "--users-table", dest="users_table", default="users.yaml",
                           help="Table of users with passwords to create WITHOUT suffixes", metavar="users.yaml")

cmd_line_parser.add_option("-s", "--suffix", dest="suffix", default="test",
                           help="Table of users with passwords to create WITHOUT suffixes", metavar="USERS.yaml")

cmd_line_parser.add_option("--create-boltek-dfclient", dest="create_boltek_dfclient", action="store_true",
                           help="Create tables for dfclient based on boltek device")

cmd_line_parser.add_option("--create-server", dest="create_server", action="store_true",
                           help="Create tables for strike processing on server")

cmd_line_options, cmd_line_args = cmd_line_parser.parse_args()

database_prefix = "lightning_detection_system"
suffix = "_" + cmd_line_options.suffix

###########################################################################################
## Doing all the job below
users = []
try:
    users = yaml.load(open(cmd_line_options.users_table))
except (OSError, IOError) as e:
    print("Cannot load file with users list: {}".format(e))
    sys.exit(0)

print(users)

for user in users:
    if "is_admin" in user and user["is_admin"] is True:
        print("hi")



auth = mysqlutils.MysqlConnectPars(cmd_line_options.root_name, cmd_line_options.root_password, cmd_line_options.hostname)
