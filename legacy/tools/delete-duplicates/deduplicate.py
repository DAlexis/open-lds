#!/usr/bin/python

import sys
import math
import array
import numpy as np
import matplotlib.pyplot as plt
from optparse import OptionParser
from datetime import timedelta
import datetime
import mysql.connector
from mysql.connector import errorcode

cmdLineParser = OptionParser()
cmdLineParser.add_option("-b", "--begin", dest="beginTime", default="2015-06-26 03:40:00",
                  help="Start time for validation", metavar="yyyy-mm-dd hh:mm:ss")
cmdLineParser.add_option("-e", "--end", dest="endTime", default="2015-06-26 03:49:00",
                  help="End time for validation", metavar="yyyy-mm-dd hh:mm:ss")
cmdLineParser.add_option("-i", "--device-id", dest="deviceId", default="3",
                  help="device id to check for duplicates", metavar="DEVICE_ID")

cmdLineParser.add_option("-H", "--host", dest="hostname", default="ldfdb.lightninglab.ru",
                  help="Database hostname", metavar="HOSTNAME")
cmdLineParser.add_option("-d", "--database", dest="database", default="boltek_flashes_raw_data",
                  help="Database name", metavar="DATABASE")
cmdLineParser.add_option("-u", "--user", dest="username", default="root",
                  help="Database user name", metavar="USERNAME")
cmdLineParser.add_option("-p", "--password", dest="password", default="",
                  help="Database password for USERNAME", metavar="PASSWORD")

(cmdLineOptions, cmdLineArgs) = cmdLineParser.parse_args()

try:
    mysqlConnection = mysql.connector.connect();
    try:
        mysqlConnection = mysql.connector.connect(
            user = cmdLineOptions.username,
            password = cmdLineOptions.password,
            host = cmdLineOptions.hostname,
            database = cmdLineOptions.database)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("Access denied: check user name or password")
        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("Database does not exists")
        raise
    
    cursor = mysqlConnection.cursor(buffered=True)
    selector = mysqlConnection.cursor(buffered=True)
    
    query = ("SELECT global_id, `when`, count_osc FROM strikes "
         "WHERE (`when` BETWEEN %s AND %s) && (device_id = %s)")
    
    print ("Selecting strikes detected by device #{} from {} at {}".format(cmdLineOptions.deviceId, cmdLineOptions.database, cmdLineOptions.hostname))
    cursor.execute(query, (cmdLineOptions.beginTime, cmdLineOptions.endTime, cmdLineOptions.deviceId))
    
    strikes = {}
    recordsCount = 0;
    for (global_id, when, count_osc) in cursor:
        recordsCount += 1
        #print ("global_id: {}, when: {}, count_osc: {}".format(global_id, when, count_osc))
        if not ((when, count_osc) in strikes):
            strikes[(when, count_osc)] = []
        strikes[(when, count_osc)].append(global_id)
    
    print("Readed {} records".format(recordsCount))
    duplicatesCount = 0
    for (when, count_osc) in strikes:
        if len(strikes[(when, count_osc)]) > 1:
            #print ("Duplicate found: {}".format(strikes[(when, count_osc)]))
            for global_id in strikes[(when, count_osc)][1:]:
                duplicatesCount+= 1
                print("To delete: {}".format(global_id))
                query = "DELETE FROM strikes WHERE global_id = {}".format(global_id)
                cursor.execute(query)
    
    print("Found {} duplicates".format(duplicatesCount))
    mysqlConnection.commit()
    print("Duplicates deleted")
    

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

finally:
    cursor.close()
    mysqlConnection.close()





