/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef UNIFIED_GPS_INFO_H_INCLUDED
#define UNIFIED_GPS_INFO_H_INCLUDED

#include "timestamp-utils.hpp"
#include "math-utils.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>

/// Manage time with seconds fractional part precision the same as double has
class Time
{
public:
    Time();
    Time(const std::string& time);
    
    std::string getRoundPart() const;
    double getFractionPart() const;
    std::string getString() const;
    
    void setTime(const std::string& round, double secondsFractional);
    
    /// Read time in format <posix date time>+<fractional seconds>, ex. "2013-11-12 18:57:03+0.230341"
    void setTime(const std::string& time);
    
    /// Really used only for random-generated strike data
    void setCurrent();
    
    double operator-(const Time& timeFrom) const;
    
    // Warning! Operators with time_duration argument should be used with only round arguments (no fraction seconds!)
    
    Time operator+(const double secondsInterval) const;
    Time operator+(const boost::posix_time::time_duration& timeDuration) const;
    
    Time operator-(const double secondsInterval) const;
    Time operator-(const boost::posix_time::time_duration& timeDuration) const;
    
    Time& operator+=(const double secondsInterval);
    Time& operator+=(const boost::posix_time::time_duration& timeDuration);
    
    Time& operator-=(const double secondsInterval);
    Time& operator-=(const boost::posix_time::time_duration& timeDuration);
    
    Time& operator=(const std::string& source);
    
    bool operator>(const Time& second) const;
    bool operator<(const Time& second) const;
    bool operator==(const Time& second) const;

private:
    PosixTime m_posixTime;
    double m_timeFractional;
};

/// Manage position with precision of double
class Position
{
public:
    Position();
    Position(double lat, double lon);
    
    char getLatitudeChar() const;
    char getLongtitudeChar() const;
    double getLatitude() const;
    double getLongitude() const;
    
    /// Set latitude; lat>0 for north hemisphere and lat<0 for south hemisphere
    void setLatitude(double lat);
    /// Set longtitude; lon>0 for east hemisphere and lon<0 for west hemisphere
    void setLongitude(double lon);
    
    /// move location to dlat in latitude and to dlon in longitude
    void move(double dlat, double dlon);
    
    /// Get latitude and longtitude by boltek notation
    void setFromBoltekData(double lat, double lon, char latChar, char lonChar);
    
    /// Get human-readable coordinates format
    std::string getString() const;
    
    /// Calculate curve connecting this point with another, in radians
    double curve(const Position& pos) const;
    
    /// Calculate azimuth in interval [-pi/2, pi/2]
    double getAzimuth(const Position& pos) const;
    
    /// Create vector on unit sphere
    void createVecotr3D(Vector3d vec) const;
    
    /// Calculate distance on sphere
    double operator-(const Position& second) const;
    
    class Exception : public std::exception {};
    
    class ExBoltekGPSDataCorrupted : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Boltek GPS data is incorrect"; }
    };
    
private:
    /// Latitude in degrees and its decimal fraction part, not ' and "
    double latitude;
    /// Longitude in degrees and its decimal frtaction part, not ' and "
    double longitude;
};

Position calculateCenterOfMass(const Position& pos1, double weight1,
                               const Position& pos2, double weight2);

#endif // UNIFIED_GPS_INFO_H_INCLUDED
