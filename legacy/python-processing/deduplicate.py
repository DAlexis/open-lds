#!/usr/bin/python

import sys
import math
import array
import numpy as np
import matplotlib.pyplot as plt
import ldfutils.utils as ut
import ldfutils.pos_time as pt
import argparse
from datetime import timedelta
import datetime
import ldfutils.connection
import mysql.connector
from mysql.connector import errorcode

parser = argparse.ArgumentParser()
parser.add_argument("--begin", "-b", dest="begin_time", default="2015-06-26 03:40:00",
                    help="Start time for validation", metavar="yyyy-mm-dd hh:mm:ss")
parser.add_argument("--end", "-e", dest="end_time", default="2015-06-26 03:49:00",
                    help="End time for validation", metavar="yyyy-mm-dd hh:mm:ss")
parser.add_argument("--device-id", "-i", dest="device_id", default="3", type=int,
                    help="device id to check for duplicates", metavar="DEVICE_ID")
parser.add_argument("--target", "-t", default="strikes", help="Database to deduplicate: 'strikes' or 'solutions'")

parser.add_argument("--save", "-s", action="store_true", help="Save data really")

parser.add_argument("--step", default="3600", type=float, help="Step in seconds for db loading chunks")

# parser.add_argument("-H", "--host", dest="hostname", default="ldfdb.lightninglab.ru",
#                     help="Database hostname", metavar="HOSTNAME")
# parser.add_argument("-d", "--database", dest="database", default="boltek_flashes_raw_data",
#                     help="Database name", metavar="DATABASE")
# parser.add_argument("-u", "--user", dest="username", default="root",
#                     help="Database user name", metavar="USERNAME")
# parser.add_argument("-p", "--password", dest="password", default="",
#                     help="Database password for USERNAME", metavar="PASSWORD")

ldfutils.connection.add_connection_settings_to_argument_parser(parser)

args = parser.parse_args()

def remove_strikes_repetitions(connection, cursor, begin_time, end_time, device_id, really_delete=False):
    select_query = ("SELECT global_id, `when`, count_osc FROM strikes WHERE "
                    + ut.ConditionGenerator.round_time_interval(begin_time, end_time, key="when") +
                   "&& (device_id = %d)" % device_id)

    print("Selecting strikes detected by device #%d from %s to %s" % (device_id, begin_time, end_time))
    cursor.execute(select_query)

    all_ids = {}
    recordsCount = 0
    for (global_id, when, count_osc) in cursor:
        recordsCount += 1
        # print ("global_id: {}, when: {}, count_osc: {}".format(global_id, when, count_osc))
        if not ((when, count_osc) in all_ids):
            all_ids[(when, count_osc)] = []
        all_ids[(when, count_osc)].append(global_id)

    print("Readed %d records" % recordsCount)
    duplicatesCount = 0
    for (when, count_osc) in all_ids:
        if len(all_ids[(when, count_osc)]) > 1:
            # print ("Duplicate found: {}".format(strikes[(when, count_osc)]))
            for global_id in all_ids[(when, count_osc)][1:]:
                duplicatesCount += 1
                print("To delete: %d" % global_id)
                delete_query = "DELETE FROM strikes WHERE global_id = %d" % global_id
                if really_delete:
                    cursor.execute(delete_query)

    print("Found {} duplicates".format(duplicatesCount))
    connection.commit()
    print("Duplicates deleted")


def remove_solutions_repetitions(connection, cursor, begin_time, end_time, device_id, really_delete=False):
    select_query = ("SELECT id, round_time, fraction_time FROM solutions WHERE " + ut.ConditionGenerator.round_time_interval(begin_time, end_time))

    print("Selecting solutions from %s to %s" % (begin_time, end_time))
    cursor.execute(select_query)

    all_ids = {}
    recordsCount = 0
    for (id, round_time, fraction_time) in cursor:
        recordsCount += 1
        key_tuple = (round_time, fraction_time)
        # print ("global_id: {}, when: {}, count_osc: {}".format(global_id, when, count_osc))
        if not (key_tuple in all_ids):
            all_ids[key_tuple] = []
        all_ids[key_tuple].append(id)

    print("Readed {} records".format(recordsCount))
    duplicatesCount = 0
    for key_tuple in all_ids:
        if len(all_ids[key_tuple]) > 1:
            # print ("Duplicate found: {}".format(strikes[(when, count_osc)]))
            for id in all_ids[key_tuple][1:]:
                duplicatesCount += 1
                print("To delete: %d" % id)
                delete_query = "DELETE FROM solutions WHERE id = %d" % id
                if really_delete:
                    cursor.execute(delete_query)

    print("Found {} duplicates".format(duplicatesCount))
    connection.commit()
    print("Duplicates deleted")

try:
    settings = {}

    ldfutils.connection.setup_connection_settings(
        settings,
        args.connection_settings,
        args.password,
        db="production"
    )
    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)

    fn_for_decomposition = None

    if args.target == "strikes":
        fn_for_decomposition = remove_strikes_repetitions
    elif args.target == "solutions":
        fn_for_decomposition = remove_solutions_repetitions
    else:
        raise ValueError("invalid --target option value: %s. 'strikes' or 'solutions' are acceptable" % args.target)

    ut.time_decomposition(
        pt.PrecisionTime(args.begin_time, 0.0),
        pt.PrecisionTime(args.end_time, 0.0),
        float(args.step),
        fn_for_decomposition,
        cursor=cursor,
        connection=conn,
        device_id=args.device_id,
        really_delete=args.save
    )
    

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

finally:
    cursor.close()
    conn.close()





