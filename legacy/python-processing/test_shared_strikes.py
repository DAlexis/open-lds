#!/usr/bin/env python3

import ldfutils.connection
import ldfutils.dbtypes as lm
import ldfutils.dbloader
import ldfutils.solutions_processing as sp
import sys
from optparse import OptionParser
import numpy as np
import matplotlib.pyplot as plt

cmd_line_parser = OptionParser()
ldfutils.connection.add_connection_settings_to_option_parser(cmd_line_parser)

cmd_line_parser.add_option("-b", "--begin", dest="begin_time", default="2014-05-01 00:00:00",
                         help="Start time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

cmd_line_parser.add_option("-e", "--end", dest="end_time", default="2017-05-01 00:00:00",
                         help="End time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

(cmd_line_options, cmdLineArgs) = cmd_line_parser.parse_args()

settings = {}

ldfutils.connection.setup_connection_settings(
    settings,
    cmd_line_options.connection_settings,
    cmd_line_options.password
)

try:
    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)

    print("Loading time clusters...")
    clusters = ldfutils.dbloader.load_time_clusters(
        cursor,
        ldfutils.dbloader.ConditionGenerator.round_time_interval(cmd_line_options.begin_time, cmd_line_options.end_time)
    )

    used_strikes = {}
    total_used_strikes_with_repetition = 0

    print("Counting used strikes...")
    for cluster in clusters:
        for strike in cluster.strikes:
            total_used_strikes_with_repetition += 1
            if strike not in used_strikes:
                used_strikes[strike] = 1
            else:
                used_strikes[strike] += 1

    unique_strikes = len(used_strikes)

    print("Unique strikes: " + str(unique_strikes) + "; total used: " + str(total_used_strikes_with_repetition))
    print("Part: " + str(float(unique_strikes)/total_used_strikes_with_repetition))
finally:
    cursor.close()
    conn.close()
