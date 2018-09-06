#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 16 16:20:10 2017

@author: dalexies
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import math
from scipy.stats import gaussian_kde

min_ratio_log = -2
max_ratio_log = 2

min_energy_log = 11
max_energy_log = 15

max_rep = 7

en_cols = [i*3+1 for i in range(0,max_rep)]

plt.rc('text', usetex=True)
plt.rc('font', family='serif')

def plot_scatter(x, y, target, vmax, title):
    bins = np.logspace(11, 15, int(0.3*math.sqrt(len(x))))
    counts, _, _ = np.histogram2d(x, y, bins=(bins, bins))
    result = target.pcolormesh(bins, bins, counts.T, vmin=0, vmax=vmax)
    target.set_xscale("log")
    target.set_yscale("log")
    target.set_title(title)
    return result

def plot_scatter_all(data):
    f, axes = plt.subplots(2, 2)
    vmax = 170
    data2=data[data[4].notnull()]
    plot_scatter(data2[1], data2[4], axes[0,0], vmax, "$I_1 / I_0$")
    data2=data[data[7].notnull()]
    plot_scatter(data2[4], data2[7], axes[0,1], vmax, "$I_2 / I_1$")
    data2=data[data[10].notnull()]
    plot_scatter(data2[7], data2[10], axes[1,0], vmax, "$I_3 / I_2$")
    data2=data[data[13].notnull()]
    pcm = plot_scatter(data2[10], data2[13], axes[1,1], vmax, "$I_4 / I_3$")
    
    f.colorbar(pcm, ax=axes.ravel().tolist())
    plt.show()

def read_data(fname):
    return pd.read_csv(fname,sep=r',', names=names, header=None, skipinitialspace=True)


def plot_parts(parts):
    
    for i in range(1,max_rep):
        d = [x for x in parts[:,i] if not math.isnan(x)]
        bins_count = int(math.sqrt(len(d)))
        plt.hist(d, bins=np.logspace(min_ratio_log, max_ratio_log, bins_count),
                 alpha=0.8, label=r'$I_'+str(i)+r'/I_'+str(i-1) + r'$')
        plt.gca().set_xscale("log")
    plt.legend()
    plt.title("Next to previous stroke current ratio")
    plt.show()

def plot_currents(data):
    for i in range(0,max_rep):
        col = i*3+1
        targets = data[col].notnull() & (data[col+1] < 0.5)
        ints = data[targets][col]
        m = np.mean(ints)
        bins_count = int(math.sqrt(ints.count()))
        plt.hist(ints, bins=np.logspace(min_energy_log, max_energy_log, bins_count),
                 alpha=0.8, label="Stroke " + str(i))
        plt.gca().set_xscale("log")
    
        #times = data[targets][col-1]
        #plt.hist(times, bins='sqrt')
        #plt.show()
        print("For {} mean is {:.2E}".format(i, m))
    
    plt.title("Currents distribution")
    plt.legend()
    plt.show()

def make_parts(data):
    parts = np.ndarray(shape=(samples_count, max_rep))
    parts[:,0] = 1
    for r in range(1,max_rep):
        parts[:, r] = data[en_cols[r]] / data[en_cols[r-1]]
    return parts

def save_stats_for_validation(data):
    first = data[1]
    others = []
    for i in en_cols[1:]:
        others.extend([x for x in data[i] if not math.isnan(x)])
    
    with open("first.txt", "w") as fp:
        for c in first:
            fp.write("%s\n" % c)
            
    with open("others.txt", "w") as fp:
        for c in others:
            fp.write("%s\n" % c)

names = list(range(0,max_rep*3))

print("Reading...")
data = read_data("/home/dalexies/Projects/lightning-df/python-processing/output-md.csv")

# Filtering too close strikes
data = data[data[3] > 0.000001]
#data = data[data[3] > 0.000001]

data = data[data[2] < 0.5]
#data = data[data[5] < 0.3]

samples_count = data.shape[0]
print("Total samples count: %d" % samples_count)

print("Calculating parts...")
parts = make_parts(data)

print("Plotting parts...")
plot_parts(parts)

print("Plotting currents...")
plot_currents(data)

print("Plotting scatter...")
plot_scatter_all(data)


#save_stats_for_validation(data)