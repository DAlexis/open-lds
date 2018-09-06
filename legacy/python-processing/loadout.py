#!/usr/bin/env python3

import ldfutils.connection
import ldfutils.dbtypes as dbt
import ldfutils.dbloader as dl
import ldfutils.utils as ut
import ldfutils.pos_time as pt
import ldfutils.solutions_processing as sp
import sys
import argparse
from typing import Dict, Tuple, List

arg_parser = argparse.ArgumentParser(description='Utility to loadout data to big csv file')
ldfutils.connection.add_connection_settings_to_argument_parser(arg_parser)

arg_parser.add_argument("-b", "--begin", dest="begin_time", default="2014-06-01 00:00:00",
                      help="Start time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

arg_parser.add_argument("-e", "--end", dest="end_time", default="2017-10-01 00:00:00",
                      help="End time for extracting", metavar="yyyy-mm-dd hh:mm:ss")

arg_parser.add_argument("-s", "--step", dest="step", default="3600",
                         help="Step in seconds for db loading chunks")

arg_parser.add_argument("-o", "--output", dest="output", default="full-solutions.csv",
                      help="CSV file for output", metavar="FILENAME.csv")

args = arg_parser.parse_args()

fields = ["time", "position", "repetitions"]

max_reps_count = 20
max_detectors_count = 10

# Delays between strikes repetitions
for i in range(1, max_reps_count+1):
    fields.append("delay"+str(i))

# Intensities of strike repetitions
for i in range(1, max_reps_count+1):
    fields.append("int"+str(i))

# Distances to detectors
for i in range(1, max_detectors_count+1):
    fields.append("dist" + str(i))

print(fields)


def create_delays_line(solutions: List[dbt.LightningMark]):
    out = ""
    last_time = solutions[0].time
    for s in solutions:
        dt = s.time - last_time
        out += str(dt) + ","
        last_time = s.time

    for _ in range(len(solutions), max_reps_count):
        out += ","
    return out


def create_intensities_line(solutions: List[dbt.LightningMark]):
    out = ""
    for s in solutions:
        out += str(s.intensity_info.intensity) + ","

    for _ in range(len(solutions), max_reps_count):
        out += ","
    return out


def create_dists_line(solutions: List[dbt.LightningMark]):
    out = ""
    first_sol = solutions[0]
    sol_details = dl.load_solutions_details(cursor, [first_sol])

    strikes = next(iter(sol_details.values())).strikes
    for s in strikes:
        d = s.position - first_sol.pos
        out += str(d) + ","

    for _ in range(len(strikes), max_detectors_count):
        out += ","
    return out


def load(connection, cursor, begin_time, end_time, outfile):
    print("Loading solutions from in interval '%s' - '%s'..." % (str(begin_time), str(end_time)))

    rep_groups = dl.load_repetition_groups(
        cursor,
        ut.ConditionGenerator.round_time_interval(begin_time, end_time)
    )

    dl.load_intensity_info_for_repetition_groups(cursor, rep_groups)

    for g in rep_groups:
        line = ""
        line += '"' + str(g.solutions[0].time) + '",'
        line += '"' + str(g.solutions[0].pos) + '",'
        line += str(len(g.solutions)) + ","
        line += create_delays_line(g.solutions)
        line += create_intensities_line(g.solutions)
        line += create_dists_line(g.solutions)
        line = line[:-1] # Removing last comma
        outfile.write(line+"\n")
        #print(line)


try:
    settings = {}

    ldfutils.connection.setup_connection_settings(
        settings,
        args.connection_settings,
        args.password,
        db="production"
    )

    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)

    with open(args.output, "w") as outfile:
        header = ""
        for f in fields:
            header += f+","
        outfile.write(header[:-1] + "\n")

        ut.time_decomposition(
            pt.PrecisionTime(args.begin_time, 0.0),
            pt.PrecisionTime(args.end_time, 0.0),
            float(args.step),
            load,
            cursor=cursor,
            connection=conn,
            outfile=outfile
        )

except:
    print("Unexpected error:", sys.exc_info()[0])
    raise

finally:
    pass
    cursor.close()
    conn.close()
