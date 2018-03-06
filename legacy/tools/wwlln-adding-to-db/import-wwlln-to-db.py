#!/usr/bin/python

from datetime import date, datetime, timedelta
import mysql.connector
from optparse import OptionParser
import csv

print ("\nWWLLN Importing tool\n")

cmdLineParser = OptionParser()
cmdLineParser.add_option("-f", "--file", dest="filename", default="",
                  help="File to be imported", metavar="FILE")
cmdLineParser.add_option("-H", "--host", dest="host", default="localhost",
                  help="MYSQL host", metavar="HOST")
cmdLineParser.add_option("-u", "--user", dest="user", default="root",
                  help="MYSQL username", metavar="USER")
cmdLineParser.add_option("-p", "--password", dest="password",
                  help="MYSQL password", metavar="PASSWORD")
cmdLineParser.add_option("-b", "--database", dest="database", default="lightning_detection_system",
                  help="MYSQL database name", metavar="DATABESE")
cmdLineParser.add_option("-l", "--files-list", dest="filesList", default="",
                  help="File with list of wwlln files", metavar="FILE_WITH_LIST")


(cmdLineOptions, cmdLineArgs) = cmdLineParser.parse_args()

addData =  ("INSERT INTO wwlln_solutions "
            "(round_time, fraction_time, lon, lat, parameter1, parameter2)"
            "VALUES (%s, %s, %s, %s, %s, %s)")

print ("Opening connection")
cnx = mysql.connector.connect(user = cmdLineOptions.user, database = cmdLineOptions.database, password = cmdLineOptions.password)
cursor = cnx.cursor()

def processFile(filename):
    print ("Reading file " + filename)
    count = 0
    with open(filename, newline='') as csvfile:
        csvReader = csv.reader(csvfile, delimiter=',', quotechar='|')
        lastTime = ""
        for row in csvReader:
            roundTime = row[0].replace("/", "-") + " " + row[1].split(".")[0]
            fractionTime = "0."+row[1].split(".")[1]
            lat = row[2]
            lon = row[3]
            par1 = row[4]
            par2 = row[5]
            data = (roundTime, fractionTime, lon, lat, par1, par2)
            if float(lat) > 47 and float(lat) < 65 and float(lon) > 35 and float(lon) < 55:
                count += 1
                cursor.execute(addData, data)
    print ("Committing to database " + str(count) + " records")
    cnx.commit()

if cmdLineOptions.filesList == "" and cmdLineOptions.filename != "":
    processFile(cmdLineOptions.filename)

if cmdLineOptions.filesList != "":
    with open(cmdLineOptions.filesList, newline='') as listFile:
        for filename in listFile:
            processFile(filename.rstrip())

cursor.close()
cnx.close()
print ("done")
