#!/usr/bin/env python3

import ldfutils.connection
import ldfutils.dbtypes as dbt
import numpy as np
import ldfutils.dbloader as dl
import ldfutils.utils as ut
import ldfutils.pos_time as pt
import ldfutils.solutions_processing as sp
import sys
import argparse


def file_name_to_device_id(file_name):
    device_index = int(file_name.split("_")[1]) + 10
    return device_index


def moscow_device_position(device_id):
    """
    Moscow device id to position
    :param device_id: 11, 12 or 13
    :return: lat, lon
    """
    if device_id == 11:
        return 55.836382, 37.555037
    if device_id == 12:
        return 55.641463, 38.374639
    if device_id == 13:
        return 55.478288, 37.311194

    raise ValueError("Moscow devcie id %d is out of range (11-13)!" % device_id)


def parse_moscow_line(line, file_name, experiment_id=1):
    """
    Формат данных в файле
    2017.08.10-11:39:40 25628313 51 155.324 384.473 384.473 34 74 -1 -1 0 188 256 188 256
    Год.месяц.день-час:минуты:секунды
    "actual frequency of timestamp's 50MHz osc"
    "TS_10ms"
    "direction"
    "distance"
    "distance_averaged"
    "North_Pk"
    "East_Pk"
    "NorthPol"
    "EastPol"
    "EFieldPol"
    "NorthMaxPos"
    "NorthMinPos"
    "EastMaxPos"
    "EastMinPos"
    :param line:
    :return:
    """
    fields = line.split(" ")
    round_time = fields[0].replace("-", " ").replace(".", "-")
    frac_time = float(fields[1]) / 50000000
    dev_id = file_name_to_device_id(file_name)
    buffer_size = 512

    lat, lon = moscow_device_position(dev_id)

    s = dbt.Strike(
        id=0,
        device_id=dev_id,
        experiment_id=experiment_id,
        E_field=[],
        MN_field=[],
        MW_field=[],
        round_time=round_time,
        fraction_time=frac_time,
        lat=lat,
        lon=lon
    )
    dl.insert_strike_raw(cursor, s)
    print(s)

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description='Utility to loadout data to big csv file')
    ldfutils.connection.add_connection_settings_to_argument_parser(arg_parser)

    arg_parser.add_argument("-i", "--input", dest="input_filename", required=True,
                          help="Input file for import", metavar="yyyy-mm-dd hh:mm:ss")

    args = arg_parser.parse_args()

    settings = {}

    ldfutils.connection.setup_connection_settings(
        settings,
        args.connection_settings,
        args.password,
        db="production"
    )

    conn, cursor = ldfutils.connection.create_connection_and_cursor(settings)
    filename = args.input_filename

    with open(filename, "r") as inpf:
        lines = inpf.readlines()

    lines = [x.strip() for x in lines]

    for l in lines:
        parse_moscow_line(l, filename)
        conn.commit()
    # for line in lines:
    #     parse_moscow_line(line)



