#!/usr/bin/python2


import sys
from time import sleep
from datetime import timedelta
import datetime
import ConfigParser
import mysql.connector
from mysql.connector import errorcode


def read_conf():
    cfg = ConfigParser.ConfigParser()
    cfg.read('db.conf')
    try:
        name = cfg.get('DEFAULT', 'name')
        host = cfg.get('DEFAULT', 'host')
        user = cfg.get('DEFAULT', 'user')
        password = cfg.get('DEFAULT', 'password')
        return name, host, user, password
    except ConfigParser.Error:
        print("Check db.conf file. Could not read configuration.")
        raise


def make_report(cursor):
    r = ""
    for (count, devid, name) in cursor:
        r += "%20s\t%d\n" % (name, count)
    return r


def make_yesterday_report(cursor):
    # this report will become obsolete after midnight
    cursor.execute("SELECT count(*), device_id, devices.name from strikes LEFT JOIN devices ON strikes.device_id=devices.id where date(`when`)<date(now()) group by device_id")
    return make_report(cursor)


def make_today_report(cursor):
    cursor.execute("SELECT count(*), device_id, devices.name from strikes LEFT JOIN devices ON strikes.device_id=devices.id where date(`when`)=date(now()) group by device_id")
    return make_report(cursor)


try:
    name, host, user, password = read_conf()
    mysql_connection = mysql.connector.connect(
        user=user,
        password=password,
        host=host,
        database=name)
    
    cursor = mysql_connection.cursor(buffered=True)
    selector = mysql_connection.cursor(buffered=True)

    yesterday_report = make_yesterday_report(cursor)
    cursor.close()
    mysql_connection.close()

    while True:
        print "Yesterday:\n%s" % yesterday_report

        mysql_connection = mysql.connector.connect(
            user=user,
            password=password,
            host=host,
            database=name)
        
        cursor = mysql_connection.cursor(buffered=True)
        selector = mysql_connection.cursor(buffered=True)

        current_report = make_today_report(cursor)
        cursor.close()
        mysql_connection.close()

        print "Today (%s):\n%s" % (datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                                   current_report)
        sleep(15*60)

    cursor.close()
    mysql_connection.close()

except ConfigParser.Error:
    pass

except mysql.connector.Error as err:
    if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
        print("Access denied: check user name or password")
    elif err.errno == errorcode.ER_BAD_DB_ERROR:
        print("Database does not exists")
    raise

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise
