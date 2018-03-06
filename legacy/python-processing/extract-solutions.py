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

cmd_line_parser.add_option("-b", "--begin", dest="begin_time", default="2016-08-01 00:00:00",
                         help="Start time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

cmd_line_parser.add_option("-e", "--end", dest="end_time", default="2017-05-01 00:00:00",
                         help="End time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

cmd_line_parser.add_option("-s", "--solutions-csv", dest="solutions_cvs", default="solutions.csv",
                         help="CSV file for solutions", metavar="FILENAME.csv")

cmd_line_parser.add_option("-H", "--histogram", dest="need_histogram", action="store_true",
                         help="Plot histogram of intervals", metavar="test/production")

(cmd_line_options, cmdLineArgs) = cmd_line_parser.parse_args()


try:
    settings = {}
    settings["return_strokes_repetitions"] = {
        "repeated_dist_max": 3000,
        "repeated_interval_max": 0.3
    }

    ldfutils.connection.setup_connection_settings(
        settings,
        cmd_line_options.connection_settings,
        cmd_line_options.password,
        db="production"
    )

    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)

    print("Loading solutions from db...")
    lightnings = ldfutils.dbloader.load_solutions(
        cursor,
        ldfutils.dbloader.ConditionGenerator.round_time_interval(cmd_line_options.begin_time, cmd_line_options.end_time)
    )

    print("Calculating intervals...")
    intervals, lightnings_vs_repetition = sp.get_repeated_strikes_intervals(
        lightnings,
        repeated_dist_max=settings["return_strokes_repetitions"]["repeated_dist_max"],
        repeated_interval_max=settings["return_strokes_repetitions"]["repeated_interval_max"]
    )

    max_repetitions_count = 13

    repetitions_count, probabilities = sp.repetition_conditional_probability(intervals, max_repetitions_count)

    # For x-axis
    repetitions = [x for x in range(0, max_repetitions_count+1)]

    plt.figure(1)
    plt.subplot(211)
    plt.plot(repetitions[1:], repetitions_count[1:])
    plt.title("Interval statistics")
    plt.xlabel("Repetitions")
    plt.ylabel("Frequency")
    plt.yscale('log')
    plt.grid(True)

    plt.subplot(212)
    plt.title("Repetition statistics")
    plt.plot(repetitions[1:], probabilities[1:])
    plt.grid(True)
    plt.show()

    print("Plotting distribution...")
    for i in range(1, 5):
        plt.hist(intervals[i], range=[0.001, max(intervals[1])], bins=100)
    plt.title("Interval statistics")
    plt.xlabel("Interval")
    plt.ylabel("Frequency")
    plt.show()
    print("done")

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

finally:
    pass
    cursor.close()
    conn.close()
