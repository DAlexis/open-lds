/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef DETECTION_TESTING_H_INCLUDED
#define DETECTION_TESTING_H_INCLUDED

#include "strikes-group.hpp"

#include <vector>

class TestingSet
{
public:
    TestingSet();
    /// Add lightning for imitation. Now only one supported
    void addLightning(double latitude, double longitude, double time, double radius);
    /// Add df with random location
    void addRandomLightning(double latFrom, double latTo,
                            double lonFrom, double lonTo,
                            unsigned int count = 1);
    /// Add location of DF station
    void addDF(double latitude, double longitude, double timeError);
    /// Add df with random location
    void addRandomDF(double latFrom, double latTo,
                     double lonFrom, double lonTo,
                     double timeDispersion,
                     unsigned int count = 1);
    
    /// Add time error to DF's data
    void generateTimeError(double timeDispersion);
    
    /// Read DFs and lightnings list from .conf file
    void readConfig(const std::string& filename);
    /// Write DFs and lightnings list to .conf file
    void writeConfig(const std::string& filename);
    
    /// Simulate lightning at specified position and fullfill strikes group class. Use it when all adding done
    void generateStrikesGroup(StrikesGroup& testGroup);
    
    ///////////////////////
    // Exceptions    
    class Exception : public std::exception {};
    
    class ExConfigReadingError : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Config file reading error"; }
    };
    
    class ExConfigWriting : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Config file writing error"; }
    };
    
private:
    class LightningImitationParameters
    {
    public:
        Position position;
        Time time;
        double radius;
        LightningImitationParameters() : radius(0) {}
    };
    
    class DFPointImitationParameters
    {
    public:
        Position position;
        Time time;
        double timeError;
        DFPointImitationParameters() : timeError(0) {}
    };
    
    /// Contains real time for the strike added with zero time
    Time startTime;
    
    std::vector<DFPointImitationParameters> DFs;
    std::vector<LightningImitationParameters> lightnings;
};

class TestingInstance
{
public:
    TestingInstance();
    
    void generateStrikesGroup();
    
    TestingSet testSet;
    StrikesGroup testGroup;
    
    ///////////////////////
    // Exceptions    
    class Exception : public std::exception {};    
    
private:
    
};

#endif
