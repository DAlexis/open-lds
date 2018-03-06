#!/usr/bin/env python3

import ldfutils.connection
import ldfutils.dbtypes as lm
import ldfutils.dbloader
import ldfutils.pos_time as pt
import ldfutils.solutions_processing as sp
import sys
from optparse import OptionParser
import numpy as np
import matplotlib.pyplot as plt

cmd_line_parser = OptionParser()
ldfutils.connection.add_connection_settings_to_option_parser(cmd_line_parser)

cmd_line_parser.add_option("-b", "--begin", dest="begin_time", default="2016-06-01 00:00:00",
                         help="Start time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

cmd_line_parser.add_option("-e", "--end", dest="end_time", default="2016-08-01 00:00:00",
                         help="End time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

cmd_line_parser.add_option("-s", "--step", dest="step", default="3600",
                         help="Step in seconds for db loading chunks")

cmd_line_parser.add_option("-f", "--filename", dest="filename", default="",
                         help="Filename to append energies", metavar="FILENAME.txt")

(cmd_line_options, cmdLineArgs) = cmd_line_parser.parse_args()


def calculate(cursor, begin_time, end_time):
    print("Loading solutions from db in interval '%s' - '%s'..." % (str(begin_time), str(end_time)))
    solutions = ldfutils.dbloader.load_solutions(
        cursor,
        ldfutils.dbloader.ConditionGenerator.round_time_interval(begin_time, end_time)
    )
    print("Solutions loaded:", len(solutions), ". Loading details...")
    full_sols = ldfutils.dbloader.load_solutions_details(
        cursor,
        solutions
    )
    print("Details loaded. Calculating energies...")
    ens = []
    for id, fsi in full_sols.items():
        #print("--- id = ", id, " ---")
        avg, tmp, dists = sp.solution_energy(fsi)
        if np.min(dists) > 60000 and np.log(avg) < 36:
            ens.append(avg)
    print("Done")
    return ens


try:
    settings = {}

    ldfutils.connection.setup_connection_settings(
        settings,
        cmd_line_options.connection_settings,
        cmd_line_options.password,
        db="production"
    )

    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)

    ens = []

    chunk_begin_time = pt.PrecisionTime(cmd_line_options.begin_time, 0.0)
    end_time = pt.PrecisionTime(cmd_line_options.end_time, 0.0)
    chunk_end_time = chunk_begin_time + float(cmd_line_options.step)
    print(str(chunk_end_time))
    print(str(end_time))

    while chunk_end_time < end_time:
        print(chunk_begin_time, " --- ", chunk_end_time)
        ens.extend(calculate(cursor, chunk_begin_time, chunk_end_time))
        chunk_begin_time = chunk_end_time
        chunk_end_time += float(cmd_line_options.step)

    ens.extend(calculate(cursor, chunk_begin_time, end_time))


    print("Total processed: ", len(ens))
    ensl = np.log(ens)

    plt.hist(ensl, bins='sqrt')
    plt.show()

    if cmd_line_options.filename != "":
        with open(cmd_line_options.filename, 'a') as f:
            for e in ens:
                f.write("%f\n" % e)

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

finally:
    pass
    cursor.close()
    conn.close()
