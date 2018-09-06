#!/usr/bin/env python3

import ldfutils.connection
import ldfutils.dbtypes as lm
import ldfutils.dbloader as dl
import ldfutils.dbwriter as dw
import ldfutils.pos_time as pt
import ldfutils.solutions_processing as sp
import ldfutils.utils as ut
import sys
from optparse import OptionParser
import numpy as np
import matplotlib.pyplot as plt

cmd_line_parser = OptionParser()
ldfutils.connection.add_connection_settings_to_option_parser(cmd_line_parser)
ldfutils.dbloader.add_time_limits_to_option_parser(cmd_line_parser)

cmd_line_parser.add_option("-s", "--step", dest="step", default="3600",
                         help="Step in seconds for db loading chunks")

(cmd_line_options, cmdLineArgs) = cmd_line_parser.parse_args()


def calculate(connection, cursor, begin_time, end_time, settings):
    print("Loading solutions from db in interval '%s' - '%s'..." % (str(begin_time), str(end_time)))
    solutions = dl.load_solutions(
        cursor,
        ut.ConditionGenerator.round_time_interval(begin_time, end_time)
    )

    print(" - Calculating intervals...")
    groups = sp.extract_repetition_groups(
        solutions,
        repeated_dist_max=settings["return_strokes_repetitions"]["repeated_dist_max"],
        repeated_interval_max=settings["return_strokes_repetitions"]["repeated_interval_max"]
    )
    print(" - Adding to database...")
    for g in groups:
        dw.write_repetition_group(cursor, g, False)

    connection.commit()
    print(" done")


try:
    settings = {}

    ldfutils.connection.setup_connection_settings(
        settings,
        cmd_line_options.connection_settings,
        cmd_line_options.password,
        db="production"
    )

    settings["return_strokes_repetitions"] = {
        "repeated_dist_max": 5000,
        "repeated_interval_max": 0.35
    }

    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)

    ut.time_decomposition(
        pt.PrecisionTime(cmd_line_options.begin_time, 0.0),
        pt.PrecisionTime(cmd_line_options.end_time, 0.0),
        float(cmd_line_options.step),
        calculate,
        cursor=cursor,
        settings=settings,
        connection=conn
    )

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

finally:
    pass
    cursor.close()
    conn.close()
