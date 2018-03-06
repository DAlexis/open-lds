/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "detection-testing.hpp"

#include "math-utils.hpp"
#include "constants.hpp"

#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>

static const std::string DFSectionPrefix = "DFStation";
static const std::string LightningSectionPrefix = "Lightning";

TestingSet::TestingSet()
{
    startTime.setCurrent();
}

void TestingSet::addLightning(double latitude, double longitude, double time, double radius)
{
    if (lightnings.empty())
        startTime.setCurrent();
    
    lightnings.push_back(LightningImitationParameters());
    lightnings.back().position.setLatitude(latitude);
    lightnings.back().position.setLongitude(longitude);
    lightnings.back().radius = radius;
    lightnings.back().time = startTime + time;
}

void TestingSet::addRandomLightning(double latFrom, double latTo,
                        double lonFrom, double lonTo,
                        unsigned int count)
{
    addLightning(Random::uniform(latFrom, latTo), Random::uniform(lonFrom, lonTo), 0, 0);
}


void TestingSet::addDF(double latitude, double longitude, double timeError)
{
    DFPointImitationParameters newDF;
    newDF.position.setLatitude(latitude);
    newDF.position.setLongitude(longitude);
    newDF.timeError = timeError;
    DFs.push_back(newDF);
}

void TestingSet::addRandomDF(double latFrom, double latTo,
                             double lonFrom, double lonTo,
                             double timeDispersion,
                             unsigned int count)
{
    for (unsigned int i=0; i<count; i++)
        addDF(Random::uniform(latFrom, latTo), Random::uniform(lonFrom, lonTo), Random::gaussian(0.0, timeDispersion));
}

void TestingSet::generateStrikesGroup(StrikesGroup& testGroup)
{
    // Time of strike
    testGroup.clear();
    for (auto it = DFs.begin(); it != DFs.end(); it++)
    {
        // TEMPORARY CODE BEGIN
        // For only one lightning
        StrikeData strike;
        strike.position = it->position;
        strike.time = lightnings.back().time + (strike.position - lightnings.back().position) / Global::LightSpeed + it->timeError;
        strike.setDeviceId(1);
        testGroup.insertStrike(strike);
        // TEMPORARY CODE END
        
        /// \todo Add here adding strikes info from different lightnings
    }
}

void TestingSet::readConfig(const std::string& filename)
{
    try {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini(filename, pt);
        int DFsCount = pt.get<int>("General.df_stations_count");
        int LightningsCount = pt.get<int>("General.lightnings_count");
        
        // Reading DFs coordinates and parameters
        for (int i = 0; i<DFsCount; i++)
        {
            std::string sectionName = DFSectionPrefix + std::to_string(i);
            if (not pt.get<bool>(sectionName + ".enabled", true))
                continue;
            addDF(pt.get<double>(sectionName + ".latitude"), pt.get<double>(sectionName + ".longitude"),
                  pt.get<double>(sectionName + ".time_error"));
        }
        
        // Reading lightnings coordinates and parameters
        for (int i = 0; i<LightningsCount; i++)
        {
            std::string sectionName = LightningSectionPrefix + std::to_string(i);
            if (not pt.get<bool>(sectionName + ".enabled", true))
                continue;
            
            addLightning(pt.get<double>(sectionName+ ".latitude"), pt.get<double>(sectionName+ ".longitude"),
                         pt.get<double>(sectionName+ ".time_shift"), pt.get<double>(sectionName+ ".radius"));
        }
    }
    catch(boost::property_tree::ini_parser::ini_parser_error &exception) {
        std::cerr << "Parsing error in " << exception.filename()
                << ":" << exception.line() << " - " << exception.message() << std::endl;
        throw ExConfigReadingError();
    }
    catch(boost::property_tree::ptree_error &exception) {
        std::cerr << "Parsing error: " << exception.what() << std::endl;
        throw ExConfigReadingError();
    }
    catch(...) {
        throw ExConfigReadingError();
    }
}

void TestingSet::writeConfig(const std::string& filename)
{
    std::ofstream file(filename);
    file << "[General]" << std::endl;
    file << "df_stations_count = " << DFs.size() << std::endl;
    file << "lightnings_count = " << lightnings.size() << std::endl;
    file << std::endl;
    
    // Writing DFs data
    for (unsigned int i = 0; i != DFs.size(); i++)
    {
        file << "[" << DFSectionPrefix + std::to_string(i) << "]" << std::endl;
        file << "latitude = " << DFs[i].position.getLatitude() << std::endl;
        file << "longitude = " << DFs[i].position.getLongitude() << std::endl;
        file << "time_error = " << DFs[i].timeError << std::endl;
        file << std::endl;
    }
    
    // Writing lightnings data
    for (unsigned int i = 0; i != lightnings.size(); i++)
    {
        file << "[" << LightningSectionPrefix + std::to_string(i) << "]" << std::endl;
        file << "latitude = " << lightnings[i].position.getLatitude() << std::endl;
        file << "longitude = " << lightnings[i].position.getLongitude() << std::endl;
        file << "time_shift = 0.0" << std::endl;
        file << "radius = 0.0" << std::endl;
        file << std::endl;
    }
    file.close();
}

void TestingSet::generateTimeError(double timeDispersion)
{
    for (auto it = DFs.begin(); it != DFs.end(); it++)
    {
        it->timeError = Random::gaussian(0.0, timeDispersion);
    }
}

TestingInstance::TestingInstance()
{
}

void TestingInstance::generateStrikesGroup()
{
    testSet.generateStrikesGroup(testGroup);
}
