#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May 28 19:42:40 2018

@author: dalexies
"""

import pandas as pd
import matplotlib.pyplot as plt

def dist(index):
    return "dist"+str(index)

def delay(index):
    return "delay"+str(index)

def intense(index):
    return "int"+str(index)

print("Reading...")
df = pd.read_csv('/home/dalexies/Projects/lightning-df/python-processing/full-solutions.csv', sep=',')
#df = df[df["time"] >= "2016"]

rep_array = []
rep_count = []
part = [0]

max_reps = 12

print("Counting...")
for i in range(1, max_reps):
    rep_count.append(i)
    rep_array.append(sum(df["repetitions"] >= i))
    if i != 1:
        part.append(float(rep_array[-1]) / rep_array[-2])
    
print("Plotting...")

plt.rc('text', usetex=True)
plt.rc('text.latex',unicode=True)
plt.rc('text.latex', preamble='\\usepackage[utf8]{inputenc}')
plt.rc('text.latex',preamble='\\usepackage[russian]{babel}')
plt.rc('font', family='serif')

fig, ax = plt.subplots()
ax.set_yscale("log", nonposy='clip')
axes = plt.gca()
axes.set_xlim([0,12])
plt.xticks(range(1,12))

plt.xlabel('Количество обратных ударов, не менее, $i$')
plt.ylabel('Зарегистрировано событий $N_i$')

plt.plot(rep_count, rep_array, 'b+', mew=2, ms=10)
plt.grid(True, 'major', 'x')
plt.grid(True, 'both', 'y')


plt.savefig("repetitions.png", dpi=300)
plt.show()

fig, ax = plt.subplots()

axes = plt.gca()
axes.set_ylim([0.0,1.0])
axes.set_xlim([0,12])
plt.xticks(range(1,12))

plt.xlabel('Номер обратного удара $i$')
plt.ylabel('Условная вероятность события $P(i\,|\,i-1)$')


plt.plot(rep_count[1:], part[1:], 'b^')

plt.grid(True)

plt.savefig("probs.png", dpi=300)
plt.show()

# Plotting average graphs

plt.figure()

means = []
medians = []
stds = []
for i in range (1, max_reps):
    means.append(df[df[delay(i)] > 0.005][delay(i)].mean())
    medians.append(df[df[delay(i)] > 0.005][delay(i)].median())
    stds.append(df[df[delay(i)] > 0.005][delay(i)].std())
    #print("N: %d, mean: %f, median: %f, std: %f" % (i, df[df[delay(i)] > 0.005][delay(i)].mean(), df[df[delay(i)] > 0.005

axes = plt.gca()
axes.set_xlim([0,12])
plt.xticks(range(1,12))

plt.plot(rep_count[1:], means[1:], 's--', label="Средний интеравал $\overline {\Delta t_i}$")
plt.plot(rep_count[1:], medians[1:], '+--', label="Медианный интервал $\Delta t_i^m$", mew=2, ms=10)
plt.plot(rep_count[1:], stds[1:], '^--', label="Среднеквадратичное отклонение $\sigma_i$")
plt.grid(True)
plt.legend()
plt.savefig("graphs.png", dpi=300)

plt.xlabel('Номер обратного удара $i$')
plt.ylabel('Время, с')

plt.show()

# Plotting historgam
delay2 = list(df[df["delay2"] > 0.005]["delay2"])
delay3 = list(df[df["delay3"] > 0.005]["delay3"])
delay4 = list(df[df["delay4"] > 0.005]["delay4"])
delay5 = list(df[df["delay5"] > 0.005]["delay5"])


#fig, ax = plt.subplots()
plt.figure(figsize=(5, 8))
plt.subplot(411)
plt.hist(delay2, bins='auto', density=True, stacked=True, facecolor='r', alpha=0.8, label="Перед компонентой №2")
plt.ylabel('Плотность вероятности')
plt.legend()
plt.subplot(412)
plt.hist(delay3, bins='auto', density=True, stacked=True, facecolor='g', alpha=0.8, label="Перед компонентой №3")
plt.ylabel('Плотность вероятности')
plt.legend()
plt.subplot(413)
plt.hist(delay4, bins='auto', density=True, stacked=True, facecolor='b', alpha=0.8, label="Перед компонентой №4")
plt.ylabel('Плотность вероятности')
plt.legend()
plt.subplot(414)
plt.hist(delay5, bins='auto', density=True, stacked=True, facecolor='c', alpha=0.9, label="Перед компонентой №5")
plt.legend()
plt.xlabel('Интервал между разрядами')
plt.ylabel('Плотность вероятности')


plt.savefig("hist.png", dpi=300)
plt.show()