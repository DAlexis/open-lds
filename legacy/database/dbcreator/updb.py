#!/usr/bin/env python3

from optparse import OptionParser
import mysqlutils
from tablesCreator import *

import mysql.connector
import os
import sys

cmdLineParser = OptionParser()

test_password = "123qweQWE"

# Note:
# 'root' means user we use to manipulate with db
# 'admin' means user we may create with root-equivalent permissions if specify -E

# Connection
cmdLineParser.add_option("-H", "--host", dest="hostname", default="localhost",
                         help="Database hostname", metavar="HOSTNAME")

cmdLineParser.add_option("-u", "--user", dest="root_name", default="admin",
                         help="Mysql user (use instead of root to create tables and users)", metavar="USERNAME")

cmdLineParser.add_option("-p", "--password", dest="root_password",  default=test_password,
                         help="Mysql root password", metavar="PASSWORD")

cmdLineParser.add_option("-z", "--production", dest="production", action="store_true",
                         help="Do operations with production database")

cmdLineParser.add_option("-Q", "--processing-host", dest="processing_host", default="localhost",
                         help="Hostname or IP of data processing server. Used for users creation")

# Users' names
cmdLineParser.add_option("-d", "--data-provider-user", dest="provider_user", default="data-provider",
                         help="Mysql user for data providers", metavar="USERNAME")

cmdLineParser.add_option("-r", "--data-processor-user", dest="processor_user", default="data-processor",
                         help="Mysql user for data processors", metavar="USERNAME")

cmdLineParser.add_option("-w", "--web-interface-user", dest="web_interface_user", default="ldn-interface",
                         help="Mysql user for web interface user", metavar="USERNAME")

cmdLineParser.add_option("-c", "--dfclient-user", dest="dfclient_user", default="dfclient",
                         help="Mysql user for web interface user", metavar="USERNAME")

cmdLineParser.add_option("-s", "--superuser-name", dest="superuser_name", default="admin",
                         help="Superuser name. Will be created with -E option", metavar="USERNAME")

# Users' passwords
default_test_password = "ldf_test"

cmdLineParser.add_option("-D", "--data-provider-password", dest="provider_password", default=default_test_password,
                         help="Mysql password for data providers", metavar="PASSWORD")

cmdLineParser.add_option("-R", "--data-processor-password", dest="processor_password", default=default_test_password,
                         help="Mysql password for data processors", metavar="PASSWORD")

cmdLineParser.add_option("-W", "--web-interface-password", dest="web_interface_password", default=default_test_password,
                         help="Mysql password for web interface user", metavar="PASSWORD")

cmdLineParser.add_option("-C", "--dfclient-password", dest="dfclient_password", default=default_test_password,
                         help="Mysql password for web interface user", metavar="PASSWORD")

cmdLineParser.add_option("-P", "--superuser-password", dest="superuser_password_new",
                         help="New superuser password. Will be created with -E option", metavar="PASSWORD")

# Job
cmdLineParser.add_option("-E", "--create-superuser", dest="set_mysql_root_password", action="store_true",
                         help="Set mysql root password with 'mysqladmin -u root PASSWORD'")

cmdLineParser.add_option("-B", "--create-boltek-dfclient", dest="create_boltek_dfclient", action="store_true",
                         help="Create tables for dfclient")

cmdLineParser.add_option("-S", "--create-server", dest="create_server", action="store_true",
                         help="Create tables for strike processing on server")

cmdLineParser.add_option("--clear-server", dest="clear_server", action="store_true",
                         help="Remove all solutions, time clusters and stats from server except strikes table")

cmdLineParser.add_option("--move-tables-to-test", dest="move_tables", action="store_true",
                         help="Copy strikes, devices, experiments tables from production to test")

cmdLineParser.add_option("--move-dev-exp-to-test", dest="move_dev_exp", action="store_true",
                         help="Move `device` and `experiments` tables to test db from production")

cmdLineParser.add_option("--sync-strikes-table", dest="sync_strikes", action="store_true",
                         help="Synchronize strikes table in test db to be up to date with production db. Operation may be slow, full copy with --move-tables-to-test may be better")

cmdLineParser.add_option("--create-table-by-name", dest="create_table_by_name", default="",
                         help="Create table by its name. For `strikes` use `strikes_server` of `strikes_dfclient`")

(cmdLineOptions, cmdLineArgs) = cmdLineParser.parse_args()

database_prefix = "lightning_detection_system"
database_production = database_prefix
database_test = database_prefix + "_test"
database = database_production
if cmdLineOptions.production is None:
    database = database_test

auth = mysqlutils.MysqlConnectPars(cmdLineOptions.root_name, cmdLineOptions.root_password, cmdLineOptions.hostname)


###########################################################################################
## Doing all the job below

if cmdLineOptions.set_mysql_root_password is not None:
    if cmdLineOptions.superuser_password_new is None:
        print("You must set password for superuser '" + cmdLineOptions.superuser_name + "' with -P")
        exit(-1)
    user_host = "'" + cmdLineOptions.superuser_name + "'@'localhost'"
    dropUser = "DROP USER IF EXISTS " + user_host
    crtUser = "CREATE USER '" + cmdLineOptions.superuser_name + "'@'localhost' IDENTIFIED BY '" + \
              cmdLineOptions.superuser_password_new + "'"
    grantPr = "GRANT ALL PRIVILEGES ON * . * TO '" + cmdLineOptions.superuser_name + "'@'localhost' WITH GRANT OPTION"
    flushPr = "FLUSH PRIVILEGES"
    # There are two variants: root can access mysql via TCP connection or via UNIX socket connection.
    # There are two variants: root can access mysql via TCP connection or via UNIX socket connection.
    # For TCP we can simply login as root (we should know its password):
    if cmdLineOptions.root_password is not None:
        print("You specified password (-p) so we are connecting via TCP")
        cnx, cursor = auth.get_connection_and_cursor()
        cursor.execute(dropUser)
        cursor.execute(crtUser)
        cursor.execute(grantPr)
        cursor.execute(flushPr)
        cnx.commit()
        cursor.close()
        cnx.close()
    else:
        # For Unix socket we should run `sudo mysql -u root` and do anything what we want
        print(crtUser)
        print("You did not specified password (-p) so we are connecting via UNIX socket")
        commands = crtUser + ";\n" + grantPr + ";\n" + flushPr + ";\n"
        os.system("echo \"" + commands + "\" | sudo mysql -u root")
    sys.exit()

if cmdLineOptions.root_password is None:
    print("Error: Mysql root password was not set, exiting")
    sys.exit(-1)

if cmdLineOptions.create_boltek_dfclient:
    if cmdLineOptions.production is not None and cmdLineOptions.dfclient_password == default_test_password:
        print(
            "Error: default password for `" + cmdLineOptions.dfclient_user + "` user can not be set for production. Use --dfclient-password")
        sys.exit(-1)

    mysqlutils.create_database(auth, database)

    tc = TablesCreator()
    tc.create(auth, database, "strikes_dfclient")
    create_user(user=cmdLineOptions.dfclient_user, password=cmdLineOptions.dfclient_password)
    permit(user=cmdLineOptions.dfclient_user, table="strikes", database=database, permissions="SELECT,INSERT", host="localhost")

if cmdLineOptions.create_server:
    if cmdLineOptions.production is not None and cmdLineOptions.processor_password == default_test_password:
        print(
            "Error: default password for `" + cmdLineOptions.processor_user + "` user can not be set for production. Use --dfclient-password")
        sys.exit(-1)

    create_database(auth, database)
    tc = TablesCreator()
    tc.create(auth, database, "time_clusters")
    tc.create(auth, database, "unified_strikes")
    tc.create(auth, database, "solutions")
    tc.create(auth, database, "solutions_energy")
    tc.create(auth, database, "strikes_stats")
    tc.create(auth, database, "strikes_server")
    tc.create(auth, database, "devices")
    tc.create(auth, database, "experiments")

    print("Server tables created, setting up users...")

    create_user(auth, user=cmdLineOptions.processor_user, password=cmdLineOptions.processor_password,
                host=cmdLineOptions.processing_host)

    permit(auth, user=cmdLineOptions.processor_user, table="unified_strikes", permissions="SELECT,INSERT", database=database,
           host=cmdLineOptions.processing_host)
    permit(auth, user=cmdLineOptions.processor_user, table="time_clusters", permissions="SELECT,INSERT", database=database,
           host=cmdLineOptions.processing_host)
    permit(auth, user=cmdLineOptions.processor_user, table="solutions", permissions="SELECT,INSERT", database=database,
           host=cmdLineOptions.processing_host)
    permit(auth, user=cmdLineOptions.processor_user, table="strikes_stats", permissions="SELECT,INSERT", database=database,
           host=cmdLineOptions.processing_host)
    permit(auth, user=cmdLineOptions.processor_user, table="strikes", permissions="SELECT", database=database,
           host=cmdLineOptions.processing_host)
    permit(auth, user=cmdLineOptions.processor_user, table="devices", permissions="SELECT,INSERT", database=database,
           host=cmdLineOptions.processing_host)
    permit(auth, user=cmdLineOptions.processor_user, table="experiments", permissions="SELECT,INSERT", database=database,
           host=cmdLineOptions.processing_host)

    create_user(auth, user=cmdLineOptions.web_interface_user, password=cmdLineOptions.web_interface_password,
                host=cmdLineOptions.processing_host)

    permit(auth, user=cmdLineOptions.web_interface_user, table="unified_strikes", permissions="SELECT", database=database)
    permit(auth, user=cmdLineOptions.web_interface_user, table="time_clusters", permissions="SELECT", database=database)
    permit(auth, user=cmdLineOptions.web_interface_user, table="solutions", permissions="SELECT", database=database)
    permit(auth, user=cmdLineOptions.web_interface_user, table="strikes_stats", permissions="SELECT", database=database)
    permit(auth, user=cmdLineOptions.web_interface_user, table="devices", permissions="SELECT", database=database)
    permit(auth, user=cmdLineOptions.web_interface_user, table="experiments", permissions="SELECT", database=database)

if cmdLineOptions.move_tables or cmdLineOptions.move_dev_exp:

    tc = TablesCreator()
    tc.create(auth, database, "devices")
    tc.create(auth, database, "experiments")

    copy_table(auth, source=database_production + ".experiments", destination=database_test + ".experiments")
    copy_table(auth, source=database_production + ".devices", destination=database_test + ".devices")

    if cmdLineOptions.move_tables:
        # This line works slow and require too much memory, so we use stupid dumping
        # copy_table(cnx, cursor, source=database_production + ".strikes", destination=database_test + ".strikes")
        copy_replace_table_large(auth, source_db=database_production, source_table="strikes", dest_db=database_test)

    print("All copying done")

if cmdLineOptions.sync_strikes:
    cnx, cursor = auth.get_connection_and_cursor()

    print("Synchronization test.strikes tables with production state...")
    cursor.execute("SELECT max(`when`) FROM " + database_test + ".strikes")
    (last_date,) = cursor.fetchone()
    print("Latest test strike was registered at " + str(last_date))
    cursor.execute(
        "INSERT INTO " + database_test + ".strikes (SELECT * FROM " + database_production + ".strikes AS t WHERE t.`when` > \"" + str(
            last_date) + "\")")
    cnx.commit()
    print("Sync done")
    cursor.close()
    cnx.close()

if cmdLineOptions.create_table_by_name != "":
    tc = TablesCreator()
    tc.create(auth, database, cmdLineOptions.create_table_by_name)

if cmdLineOptions.clear_server:
    tc = TablesCreator()
    tc.create(auth, database, "time_clusters")
    tc.create(auth, database, "unified_strikes")
    tc.create(auth, database, "solutions")
    tc.create(auth, database, "strikes_stats")
