#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import MySQLdb
import numpy
import matplotlib
import matplotlib.pyplot
from db_config import config


def get_mysql_connection():
    return MySQLdb.connect(host=config['ldf']['host'],
                           user=config['ldf']['user'],
                           passwd=config['ldf']['pass'],
                           db=config['ldf']['name'])


def blob_to_list(blob_data):
    return [ord(blob_data[i * 4]) for i in range(0, len(blob_data) / 4)]


def blob_to_numpy_array(blob_data):
    return numpy.array(blob_to_list(blob_data))


def get_fields(id):
    try:
        con = get_mysql_connection()
        cur = con.cursor()
        q = 'SELECT `E_field`,`MN_field`,`MW_field` FROM `strikes` WHERE `global_id`=%d' % id
        cur.execute(q)
        result = cur.fetchone()
        return blob_to_numpy_array(result[0]), \
               blob_to_numpy_array(result[1]), \
               blob_to_numpy_array(result[2])

    except MySQLdb.Error as err:
        print err
        return None, None, None


def plot_fields(id):
    e, mn, me = get_fields(id)

    if e.size != mn.size:
        raise Exception('plot_fields', 'e.size and mn.size differs')
    if e.size != me.size:
        raise Exception('plot_fields', 'e.size and mn.size differs')

    matplotlib.pyplot.rc('font', family='serif')

    fig = matplotlib.pyplot.figure()
    ax = fig.add_subplot(111)

    tt = numpy.arange(0, e.size, 1) / 512.0 * 64.0

    ax.plot(tt, e, 'b')
    ax.plot(tt, mn, 'r')
    ax.plot(tt, me, 'g')

    ax.set_xlabel('time, mks')
    ax.set_ylabel('electric/magnetic field, a.e.')
    ax.legend(['E', 'H1', 'H2'], loc=2)

    matplotlib.pyplot.savefig('EMF-%d.png' % id)
    matplotlib.pyplot.show()
    #matplotlib.pyplot.clf()


if __name__ == '__main__':
    if len(sys.argv) == 2:
        plot_fields(int(sys.argv[1]))
    else:
        print "Usage: plot_fields.py <id>"
