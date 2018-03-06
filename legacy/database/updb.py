#!/usr/bin/env python3

from optparse import OptionParser
import mysql.connector
import os
import sys

cmdLineParser = OptionParser()

# Note:
# 'root' means user we use to manipulate with db
# 'admin' means user we may create with root-equivalent permissions if specify -E

# Connection
cmdLineParser.add_option("-H", "--host", dest="hostname", default="localhost",
                  help="Database hostname", metavar="HOSTNAME")

cmdLineParser.add_option("-u", "--user", dest="root_name", default="admin",
                  help="Mysql user (use instead of root to create tables and users)", metavar="USERNAME")

cmdLineParser.add_option("-p", "--password", dest="root_password",
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
# todo drop all
cmdLineParser.add_option("-E", "--create-superuser", dest="set_mysql_root_password", action="store_true",
                  help="Set mysql root password with 'mysqladmin -u root PASSWORD'")

cmdLineParser.add_option("-B", "--create-boltek-dfclient", dest="create_boltek_dfclient", action="store_true",
                  help="Create tables for dfclient")

cmdLineParser.add_option("-S", "--create-server", dest="create_server", action="store_true",
                  help="Create tables for strike processing on server")

cmdLineParser.add_option("--move-tables-to-test", dest="move_tables", action="store_true",
                  help="Copy strikes, devices, experiments tables from production to test")

cmdLineParser.add_option("--move-dev-exp-to-test", dest="move_dev_exp", action="store_true",
                  help="Move `device` and `experiments` tables to test db from production")

cmdLineParser.add_option("--sync-strikes-table", dest="sync_strikes", action="store_true",
                  help="Synchronize strikes table in test db to be up to date with production db. Operation may be slow, full copy with --move-tables-to-test may be better")

(cmdLineOptions, cmdLineArgs) = cmdLineParser.parse_args()

database_prefix = "lightning_detection_system"
database_production = database_prefix
database_test = database_prefix + "_test"
database = database_production
if cmdLineOptions.production is None:
    database = database_test


def copy_table(cnx, cursor, source, destination):
    print("Copying " + source + " to " + destination + "...")
    cursor.execute("INSERT INTO " + destination + " SELECT * FROM " + source)
    cnx.commit()
    print("done")


def copy_replace_table_large(source_db, source_table, dest_db):
    source = source_db + "." + source_table
    print("Copying " + source + " to " + dest_db + " with musqldump...")
    print("dumping...")
    temp_file_name = "/tmp/table_dump.sql"
    os.system("mysqldump -u " + cmdLineOptions.root_name +
              " -p" + cmdLineOptions.root_password +
              " -h " + cmdLineOptions.hostname +
              " " + source_db +
              " " + source_table +
              " > " + temp_file_name)

    print("inserting...")
    os.system("mysql -u " + cmdLineOptions.root_name +
              " -p" + cmdLineOptions.root_password +
              " -h " + cmdLineOptions.hostname +
              " " + dest_db +
              " < " + temp_file_name)

    print("done")


def create_database():
    print("Creating schema...")
    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()
    cursor.execute("CREATE SCHEMA IF NOT EXISTS " + database + " DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci")
    cnx.commit()
    cursor.close()
    cnx.close()
    print("done")


def create_user(user, password, host="localhost"):
    user_host = "'" + user + "'@'" + host + "'"
    print("Re-creating " + user_host + "...")
    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()
    try:
        cursor.execute("DROP USER " + user_host)
    except mysql.connector.errors.DatabaseError:
        pass
    
    cursor.execute("CREATE USER " + user_host + " IDENTIFIED BY '" + password + "'")
    cnx.commit()
    print("done")
    cursor.close()
    cnx.close()


def permit(user, table, permissions, host="localhost"):
    user_host = "'" + user + "'@'" + host + "'"
    print("Granting " + permissions + " permissions for " + user_host + " on `" + table + "`...")
    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()
    cursor.execute("GRANT " + permissions + " ON " + database + "." + table + " TO " + user_host + " WITH GRANT OPTION")
    cnx.commit()
    print("done")
    cursor.close()
    cnx.close()


def create_experiments_table(cnx, cursor, database):
    cursor.execute("DROP TABLE IF EXISTS " + database + ".`experiments`")
    cnx.commit()

    cursor.execute("""
    CREATE TABLE """ + database + """.`experiments` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `description` text NOT NULL,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8
    """)
    cnx.commit()


def create_devices_table(cnx, cursor, database):
    cursor.execute("DROP TABLE IF EXISTS " + database + ".`devices`")
    cnx.commit()

    cursor.execute("""
    CREATE TABLE """ + database + """.`devices` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `name` varchar(200) NOT NULL,
      `active` int(11),
      `lat` double,
      `lon` double,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8
    """)
    cnx.commit()

###########################################################################################
## Doing all the job below

if cmdLineOptions.set_mysql_root_password is not None:
    if cmdLineOptions.superuser_password_new is None:
        print("You must set password for superuser '" + cmdLineOptions.superuser_name + "' with -P")
        exit(-1)
    user_host = "'" + cmdLineOptions.superuser_name + "'@'localhost'"
    crtUser = "CREATE USER '" + cmdLineOptions.superuser_name + "'@'localhost' IDENTIFIED BY '" + \
        cmdLineOptions.superuser_password_new + "'"
    grantPr = "GRANT ALL PRIVILEGES ON * . * TO '" + cmdLineOptions.superuser_name + "'@'localhost' WITH GRANT OPTION"
    flushPr = "FLUSH PRIVILEGES"
    # There are two variants: root can access mysql via TCP connection or via UNIX socket connection.
    # There are two variants: root can access mysql via TCP connection or via UNIX socket connection.
    # For TCP we can simply login as root (we should know its password):
    if cmdLineOptions.root_password is not None:
        print("You specified password (-p) so we are connecting via TCP")
        cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                      database=database,
                                      password=cmdLineOptions.root_password,
                                      host=cmdLineOptions.hostname)
        cursor = cnx.cursor()
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
        print("Error: default password for `" + cmdLineOptions.dfclient_user + "` user can not be set for production. Use --dfclient-password")
        sys.exit(-1)

    create_database()

    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  database=database,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()
    print("Re-creating `strikes` table...")
    cursor.execute("DROP TABLE IF EXISTS `strikes`")
    cursor.execute("""
    CREATE TABLE `strikes` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `device_id` int(11) NOT NULL,
      `experiment_id` int(11) NOT NULL,
      `when` datetime NOT NULL,
      `osc_freq` int(11) NOT NULL,
      `count_osc` int(11) NOT NULL,
      `lon` double NOT NULL,
      `lat` double NOT NULL,
      `lon_ew` enum('E','W') NOT NULL,
      `lat_ns` enum('N','S') NOT NULL,
      `E_field` blob NOT NULL,
      `MN_field` blob NOT NULL,
      `MW_field` blob NOT NULL,
      PRIMARY KEY (`id`),
      KEY `experiment_id` (`experiment_id`),
      KEY `device_id` (`device_id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8;
    """)
    cnx.commit()
    print("done")
    cursor.close()
    cnx.close()

    create_user(user=cmdLineOptions.dfclient_user, password=cmdLineOptions.dfclient_password)

    permit(user=cmdLineOptions.dfclient_user, table="strikes", permissions="SELECT,INSERT")


if cmdLineOptions.create_server:
    if cmdLineOptions.production is not None and cmdLineOptions.processor_password == default_test_password:
        print("Error: default password for `" + cmdLineOptions.processor_user + "` user can not be set for production. Use --dfclient-password")
        sys.exit(-1)

    create_database()
    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  database=database,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()
    print("Deleting old tables...")
    cursor.execute("DROP TABLE IF EXISTS `time_clusters`")
    cursor.execute("DROP TABLE IF EXISTS `unified_strikes`")
    cursor.execute("DROP TABLE IF EXISTS `solutions`")
    cursor.execute("DROP TABLE IF EXISTS `strikes_stats`")
    cursor.execute("DROP TABLE IF EXISTS `strikes`")
    cursor.execute("DROP TABLE IF EXISTS `devices`")
    cursor.execute("DROP TABLE IF EXISTS `experiments`")

    cnx.commit()
    print("done")

    print("Creating `unified_strikes`...")
    cursor.execute("""
    CREATE TABLE `unified_strikes` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `device_id` int(11) NOT NULL,
      `experiment_id` int(11) NOT NULL,
      `round_time` datetime NOT NULL,
      `fraction_time` double NOT NULL,
      `buffer_duration` double NOT NULL,
      `buffer_values_count` int(11) NOT NULL,
      `buffer_begin_time_shift` double NOT NULL,
      `lon` double NOT NULL,
      `lat` double NOT NULL,
      `direction` double,
      `direction_error` double,
      `E_field` blob NOT NULL,
      `MN_field` blob NOT NULL,
      `MW_field` blob NOT NULL,
      PRIMARY KEY (`id`),
      KEY `experiment_id` (`experiment_id`),
      KEY `device_id` (`device_id`),
      KEY `round_time` (`round_time`),
      KEY `fraction_time` (`fraction_time`)
    ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=9
    """)
    cnx.commit()
    print("done")

    print("Creating `time_clusters`...")
    cursor.execute("""
    CREATE TABLE `time_clusters` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `round_time` datetime NOT NULL,
      `fraction_time` double NOT NULL,
      `strikes_count` int(11) NOT NULL,
      `strikes` text NOT NULL,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5
    """)
    cnx.commit()
    print("done")

    print("Creating `solutions`...")
    cursor.execute("""
    CREATE TABLE `solutions` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `round_time` datetime NOT NULL,
      `fraction_time` double NOT NULL,
      `lon` double NOT NULL,
      `lat` double NOT NULL,
      `time_cluster` int(11) NOT NULL,
      `status` TINYINT(1),
      `count_of_detections` int(11) NOT NULL,
      `dispersion` double,
      PRIMARY KEY (`id`),
      KEY `round_time` (`round_time`),
      KEY `fraction_time` (`fraction_time`),
      KEY `status` (`status`),
      KEY `count_of_detections` (`count_of_detections`)
    ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5
    """)
    cnx.commit()
    print("done")

    print("Creating `strikes_stats`...")
    cursor.execute("""
    CREATE TABLE `strikes_stats` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `round_time` datetime NOT NULL,
      -- `detections_count` int(11) NOT NULL,
      `solutions_count` int(11) NOT NULL,
      PRIMARY KEY (`id`),
      KEY `round_time` (`round_time`),
      KEY `solutions_count` (`solutions_count`)
    ) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=5
    """)
    cnx.commit()
    print("done")

    print("Creating `strikes` for remote boltek devices... TODO: change name for this table!")
    cursor.execute("""
    CREATE TABLE `strikes` (
      `global_id` int(11) NOT NULL AUTO_INCREMENT,
      `id` int(11) NOT NULL,
      `device_id` int(11) NOT NULL,
      `experiment_id` int(11) NOT NULL,
      `when` datetime NOT NULL,
      `osc_freq` int(11) NOT NULL,
      `count_osc` int(11) NOT NULL,
      `when_date` date,
      `lon` double NOT NULL,
      `lat` double NOT NULL,
      `lon_ew` enum('E','W') NOT NULL,
      `lat_ns` enum('N','S') NOT NULL,
      `E_field` blob NOT NULL,
      `MN_field` blob NOT NULL,
      `MW_field` blob NOT NULL,
      PRIMARY KEY (`global_id`),
      KEY `experiment_id` (`experiment_id`),
      KEY `device_id` (`device_id`),
      KEY `when` (`when`),
      KEY `count_osc` (`count_osc`)
    ) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8
    """)
    cnx.commit()
    print("done")

    print("Creating `devices`...")
    cursor.execute("""
    CREATE TABLE `devices` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `name` varchar(200) NOT NULL,
      `active` int(11),
      `lat` double,
      `lon` double,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8
    """)
    cnx.commit()
    print("done")

    print("Creating `experiments`...")
    cursor.execute("""
    CREATE TABLE `experiments` (
      `id` int(11) NOT NULL AUTO_INCREMENT,
      `description` text NOT NULL,
      PRIMARY KEY (`id`)
    ) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8
    """)
    cnx.commit()
    print("done")

    cursor.close()
    cnx.close()

    create_user(user=cmdLineOptions.processor_user, password=cmdLineOptions.processor_password, host=cmdLineOptions.processing_host)

    permit(user=cmdLineOptions.processor_user, table="unified_strikes", permissions="SELECT,INSERT", host=cmdLineOptions.processing_host)
    permit(user=cmdLineOptions.processor_user, table="time_clusters",   permissions="SELECT,INSERT", host=cmdLineOptions.processing_host)
    permit(user=cmdLineOptions.processor_user, table="solutions",       permissions="SELECT,INSERT", host=cmdLineOptions.processing_host)
    permit(user=cmdLineOptions.processor_user, table="strikes_stats",   permissions="SELECT,INSERT", host=cmdLineOptions.processing_host)
    permit(user=cmdLineOptions.processor_user, table="strikes",         permissions="SELECT",        host=cmdLineOptions.processing_host)
    permit(user=cmdLineOptions.processor_user, table="devices",         permissions="SELECT,INSERT", host=cmdLineOptions.processing_host)
    permit(user=cmdLineOptions.processor_user, table="experiments",     permissions="SELECT,INSERT", host=cmdLineOptions.processing_host)

    create_user(user=cmdLineOptions.web_interface_user, password=cmdLineOptions.web_interface_password, host=cmdLineOptions.processing_host)

    permit(user=cmdLineOptions.web_interface_user, table="unified_strikes", permissions="SELECT")
    permit(user=cmdLineOptions.web_interface_user, table="time_clusters",   permissions="SELECT")
    permit(user=cmdLineOptions.web_interface_user, table="solutions",       permissions="SELECT")
    permit(user=cmdLineOptions.web_interface_user, table="strikes_stats",   permissions="SELECT")
    permit(user=cmdLineOptions.web_interface_user, table="devices",         permissions="SELECT")
    permit(user=cmdLineOptions.web_interface_user, table="experiments",     permissions="SELECT")


if cmdLineOptions.move_tables or cmdLineOptions.move_dev_exp:
    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()

    create_devices_table(cnx, cursor, database_test)
    create_experiments_table(cnx, cursor, database_test)

    cnx.commit()

    copy_table(cnx, cursor, source=database_production + ".experiments", destination=database_test + ".experiments")
    copy_table(cnx, cursor, source=database_production + ".devices", destination=database_test + ".devices")

    if cmdLineOptions.move_tables:
        # This line works slow and require too much memory, so we use stupid dumping
        #copy_table(cnx, cursor, source=database_production + ".strikes", destination=database_test + ".strikes")

        copy_replace_table_large(source_db=database_production, source_table="strikes", dest_db=database_test)

    print("All copying done")
    cursor.close()
    cnx.close()

if cmdLineOptions.sync_strikes:
    cnx = mysql.connector.connect(user=cmdLineOptions.root_name,
                                  password=cmdLineOptions.root_password,
                                  host=cmdLineOptions.hostname)
    cursor = cnx.cursor()
    print("Synchronization test.strikes tables with production state...")
    cursor.execute("SELECT max(`when`) FROM " + database_test + ".strikes")
    (last_date, ) = cursor.fetchone()
    print("Latest test strike was registered at " + str(last_date))
    cursor.execute("INSERT INTO " + database_test + ".strikes (SELECT * FROM " + database_production + ".strikes AS t WHERE t.`when` > \"" + str(last_date) + "\")")
    cnx.commit()
    print("Sync done")
    cursor.close()
    cnx.close()
