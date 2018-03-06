#!/usr/bin/python

import sys
import math
import utils
import array
import numpy as np
import matplotlib.pyplot as plt
from optparse import OptionParser
from datetime import timedelta
import datetime
import mysql.connector
from mysql.connector import errorcode

cmdLineParser = OptionParser()
cmdLineParser.add_option("-b", "--begin", dest="beginTime", default="2014-07-20 00:00:00",
                  help="Start time for validation", metavar="yyyy-mm-dd hh:mm:ss")
cmdLineParser.add_option("-e", "--end", dest="endTime", default="2014-10-02 23:00:00",
                  help="End time for validation", metavar="yyyy-mm-dd hh:mm:ss")

cmdLineParser.add_option("-H", "--host", dest="hostname", default="localhost",
                  help="Database hostname", metavar="HOSTNAME")
cmdLineParser.add_option("-d", "--database", dest="database", default="lightning_detection_system",
                  help="Database name", metavar="DATABASE")
cmdLineParser.add_option("-u", "--user", dest="username", default="root",
                  help="Database user name", metavar="USERNAME")
cmdLineParser.add_option("-p", "--password", dest="password", default="",
                  help="Database password for USERNAME", metavar="PASSWORD")

cmdLineParser.add_option("-t", "--time-epsilon", dest="timeEpsilon", default="0.001", type="float",
                  help="Time vicinity near WWLLN strikes", metavar="VICINITY")
cmdLineParser.add_option("-s", "--time-shift", dest="timeShift", default="-1", type="float",
                  help="TIME_SHIFT=GPS-UTC time difference", metavar="TIME_SHIFT")
cmdLineParser.add_option("-n", "--hide-not-correlated", action="store_true", dest="hideNotCorrelated",
                  help="Hide wwllnn without correlation")

cmdLineParser.add_option("--llon", dest="leftLon", default="43.39", type="float",
                  help="Left longitude border", metavar="LONGITUDE")
cmdLineParser.add_option("--rlon", dest="rightLon", default="44.48", type="float",
                  help="Right longitude border", metavar="LONGITUDE")
                  
cmdLineParser.add_option("--blat", dest="bottomLat", default="56.26", type="float",
                  help="Bottom latitude border", metavar="LATITUDE")
cmdLineParser.add_option("--tlat", dest="topLat", default="56.97", type="float",
                  help="Top latitude border", metavar="LATITUDE")


(cmdLineOptions, cmdLineArgs) = cmdLineParser.parse_args()

try:
    mysqlConnection = mysql.connector.connect();
    try:
        mysqlConnection = mysql.connector.connect(
            user = cmdLineOptions.username,
            password = cmdLineOptions.password,
            host = cmdLineOptions.hostname,
            database = cmdLineOptions.database)
    except mysql.connector.Error as err:
        if err.errno == errorcode.ER_ACCESS_DENIED_ERROR:
            print("Access denied: check user name or password")
        elif err.errno == errorcode.ER_BAD_DB_ERROR:
            print("Database does not exists")
        raise
    
    cursor = mysqlConnection.cursor(buffered=True)
    selector = mysqlConnection.cursor(buffered=True)
    
    query = ("SELECT id, round_time, fraction_time, lon, lat FROM wwlln_solutions "
         "WHERE round_time BETWEEN %s AND %s")
    
    cursor.execute(query, (cmdLineOptions.beginTime, cmdLineOptions.endTime))
    
    wwllnCount = 0;
    inVicinityCount = 0;
    
    dy = array.array('d')
    dx = array.array('d')
    
    tooFarCount = 0
    
    for (id, roundTime, fractionTime, lon, lat) in cursor:
        if lon<cmdLineOptions.leftLon or lon>cmdLineOptions.rightLon or lat<cmdLineOptions.bottomLat or lat>cmdLineOptions.topLat:
            continue
        
        wwllnCount += 1
        message = 'wwllnn: id={},\ttime={},\tlat={},\tlon={}:'.format(id, roundTime, lat, lon)
        query = ("SELECT id FROM solutions"
            "WHERE round_time = '{}'".format(roundTime))
        
        (timeFromRound, timeFromFractional) = utils.moveTime(roundTime, fractionTime, -float(cmdLineOptions.timeEpsilon) + float(cmdLineOptions.timeShift))
        (timeToRound, timeToFractional) = utils.moveTime(roundTime, fractionTime, float(cmdLineOptions.timeEpsilon) + float(cmdLineOptions.timeShift))
        
        query = "SELECT id, lon, lat, round_time FROM solutions WHERE " + utils.generateTimeConditions(timeFromRound, timeFromFractional, timeToRound, timeToFractional)
        # print(query)
        selector.execute(query)
        inVicinity = False
        messagePrinted = False
        
        minDY = 0
        minDX = 0
        minDist = 0
        isMinInitialized = False
        
        for (ourId, ourLon, ourLat, ourRoundTime) in selector:
            if not messagePrinted:
                messagePrinted = True
                print(message)
            
            distance = utils.distance(lat, lon, ourLat, ourLon)
            if distance > 10000:
                tooFarCount += 1
                continue
            
            print ("    Our: {}\tdistance: {};\tlat={}\tlon={}\ttime={}".format(ourId, utils.distance(lat, lon, ourLat, ourLon), ourLat, ourLon, ourRoundTime))
            
            cdx = math.copysign(utils.distance(lat, lon, lat, ourLon), lon-ourLon)
            cdy = math.copysign(utils.distance(lat, lon, ourLat, lon), lat-ourLat)
            cdist = utils.distance(lat, lon, ourLat, ourLon)
            
            if cdist < minDist or not isMinInitialized:
                isMinInitialized = True
                minDY = cdy
                minDX = cdx
                minDist = cdist
            
            inVicinity = True
        
        if isMinInitialized:
            dx.append(minDX)
            dy.append(minDY)
        
        if not messagePrinted and not bool(cmdLineOptions.hideNotCorrelated):
            print(message)
        
        if inVicinity:
            inVicinityCount += 1
    
    mysqlConnection.close()
    
    print("WWLLN count: {}, has our solution in vicinity: {}".format(wwllnCount, inVicinityCount))
    
    averageDx = sum(dx)/len(dx)
    averageDy = sum(dy)/len(dy)
    print("Average difference:\n"
        "\tlatitude: {}m; longitude: {}m".format(averageDy, averageDx))
    print("Standard deviations:\n\tlatitude: {}m; longitude: {}m".format(np.std(dy), np.std(dx)))
    
    print("Too far strikes: {}".format(tooFarCount))
    
    plt.figure(1)
    plt.subplot(211)
    plt.hist(dy, label = "Latitude error", bins=15)
    plt.legend()
    plt.xlabel("Value")
    plt.ylabel("Frequency")
    
    plt.subplot(212)
    plt.hist(dx, label = "Longitude error", bins=15)
    plt.xlabel("Value")
    plt.ylabel("Frequency")
    plt.legend()
    
    plt.figure(2)

    #heatmap, xedges, yedges = np.histogram2d(distX, distY, bins=10)
    #extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
    #plt.imshow(heatmap, extent=extent)
    
    plt.plot(dx, dy, 'rs', label = "Differences")
    plt.xlabel("X difference")
    plt.ylabel("Y difference")
    plt.axis((-15000, 15000, -15000, 15000))
    plt.show()
    
    
    #plt.title("Longitude error")
    #plt.xlabel("Value")
    #plt.ylabel("Frequency")
    #plt.show()
    
except:
    print()
    print("Unexpected error:", sys.exc_info()[0])
    raise





