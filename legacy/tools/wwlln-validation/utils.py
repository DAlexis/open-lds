#!/usr/bin/python

import math
import datetime
from datetime import timedelta

EarthRadius = 6371000

def curveToDistance(curve):
    return curve / 180 * math.pi * EarthRadius

def sqr(a):
    return a*a;

def moveTime(roundTime, fractionTime, shift):
    fractionTime += shift
    secondsCount = math.floor(fractionTime)
    fractionTime -= secondsCount
    roundTime += datetime.timedelta(seconds=secondsCount)
    return (roundTime, fractionTime)

def generateTimeConditions(roundTimeFrom, fractionTimeFrom, roundTimeTo, fractionTimeTo):
    condition = ("(round_time>'{0}' || (round_time='{0}' && fraction_time>{1})) "
        "&& (round_time<'{2}' || (round_time='{2}' && fraction_time<{3}))")
    return condition.format(roundTimeFrom, fractionTimeFrom, roundTimeTo, fractionTimeTo)



def haversine(angle):
    return sqr(math.sin(angle/2.0));

def inverseHaversine(value):
    return 2.0*math.asin(math.sqrt(value));

def distance(lat1, lon1, lat2, lon2):
    theta = (lon2-lon1) * math.pi / 180
    phi1 = lat1 * math.pi / 180
    phi2 = lat2 * math.pi / 180
    curve = inverseHaversine (haversine(phi1-phi2) + math.cos(phi1)*math.cos(phi2)*haversine(theta))
    return curve * 6371000
