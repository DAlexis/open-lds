/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "positioning.hpp"
#include "timestamp-utils.hpp"
#include "constants.hpp"
#include "math-utils.hpp"
#include <math.h>
#include <boost/algorithm/string.hpp>

Time::Time() :
    m_timeFractional(0)
{
    setCurrent();
}

Time::Time(const std::string& time)
{
    setTime(time);
}

std::string Time::getRoundPart() const
{
    return formatTime(m_posixTime);
}

double Time::getFractionPart() const
{
    return m_timeFractional;
}

void Time::setTime(const std::string& round, double secondsFractional)
{
    m_posixTime = boost::posix_time::time_from_string(round);
    m_timeFractional = secondsFractional;
}

void Time::setTime(const std::string& time)
{
    std::vector< std::string > parts;
    boost::split(parts, time, boost::is_any_of("+"));
    setTime(parts[0], std::stod(parts[1]));
}

std::string Time::getString() const
{
    std::ostringstream result;
    result << getRoundPart() << "+" << getFractionPart();
    return result.str();
}

void Time::setCurrent()
{
    m_posixTime = boost::posix_time::second_clock::local_time();
    PosixTime timeWithMilliseconds = boost::posix_time::microsec_clock::local_time();

    m_timeFractional = (timeWithMilliseconds - m_posixTime).total_microseconds() * 1e-6;
    if (m_timeFractional < 0)
    {
    	m_timeFractional = 0;
    	*this += 1;
    }
}

double Time::operator-(const Time& timeFrom) const
{
    double result = m_timeFractional - timeFrom.m_timeFractional;
    boost::posix_time::time_duration duration =  m_posixTime - timeFrom.m_posixTime;
    result += duration.total_seconds();
    return result;
}

Time Time::operator+(const double secondsInterval) const
{
    Time result=*this;
        
    result.m_timeFractional += secondsInterval;
    
    int roundShift = floor(result.m_timeFractional);
    result.m_timeFractional -= roundShift;
    
    result.m_posixTime += boost::posix_time::seconds(roundShift);
    return result;
}

Time Time::operator+(const boost::posix_time::time_duration& timeDuration) const
{
    Time result = *this;
    result.m_posixTime += timeDuration;
    return result;
}

Time Time::operator-(const double secondsInterval) const
{
    Time result=*this;
    result = result + (-secondsInterval);
    return result;
}

Time Time::operator-(const boost::posix_time::time_duration& timeDuration) const
{
    Time result=*this;
    result = result + (-timeDuration);
    return result;
}

Time& Time::operator+=(const double secondsInterval)
{
    m_timeFractional += secondsInterval;
    
    int roundShift = floor(m_timeFractional);
    m_timeFractional -= roundShift;
    
    m_posixTime += boost::posix_time::seconds(roundShift);
    return *this;
}

Time& Time::operator+=(const boost::posix_time::time_duration& timeDuration)
{
    m_posixTime += timeDuration;
    return *this;
}

Time& Time::operator-=(const double secondsInterval)
{
    return (*this)+= -secondsInterval;
}

Time& Time::operator-=(const boost::posix_time::time_duration& timeDuration)
{
    return (*this)+= -timeDuration;
}

Time& Time::operator=(const std::string& source)
{
    setTime(source);
    return *this;
}

bool Time::operator>(const Time& second) const
{
    if (m_posixTime > second.m_posixTime)
        return true;
    if (m_posixTime == second.m_posixTime && m_timeFractional > second.m_timeFractional)
        return true;
    
    return false;    
}

bool Time::operator<(const Time& second) const
{
    if (m_posixTime < second.m_posixTime)
        return true;
    if (m_posixTime == second.m_posixTime && m_timeFractional < second.m_timeFractional)
        return true;
    
    return false;
}

bool Time::operator==(const Time& second) const
{
    if ( *this > (second-Configuration::Instance().precision.timePrecision) &&
         *this < (second+Configuration::Instance().precision.timePrecision))
         return true;
    return false;
}

Position::Position() :
    latitude(0),
    longitude(0)
{
}

Position::Position(double lat, double lon) :
    latitude(lat),
    longitude(lon)
{
}

char Position::getLatitudeChar() const
{
    if (latitude >= 0)
        return 'N';
    else
        return 'S';
}

char Position::getLongtitudeChar() const
{
    if (longitude >= 0)
        return 'E';
    else
        return 'W';
}

void Position::setFromBoltekData(double lat, double lon, char latChar, char lonChar)
{
    lat = fabs(lat);
    lon = fabs(lon);
    if (lat > 324000000)
        throw ExBoltekGPSDataCorrupted();
    if (lon > 648000000)
        throw ExBoltekGPSDataCorrupted();
        
    latitude = lat / 324000000 * 90;
    longitude = lon / 648000000 * 180;
    switch(latChar)
    {
        case 'N': break;
        case 'S': latitude = -latitude; break;
        default: throw ExBoltekGPSDataCorrupted();
    }
    switch(lonChar)
    {
        case 'E': break;
        case 'W': longitude = -longitude; break;
        default: throw ExBoltekGPSDataCorrupted();
    }
}

void Position::setLatitude(double lat)
{
    latitude = lat;
}

void Position::setLongitude(double lon)
{
    longitude = lon;
}

double Position::getLatitude() const
{
    return latitude;
}

double Position::getLongitude() const
{
    return longitude;
}

double Position::curve(const Position& second) const
{
    double theta = (second.longitude-longitude) * M_PI / 180;
    
    double phi1 = latitude * M_PI / 180;
    double phi2 = second.latitude * M_PI / 180;
    
    //double cosAlpha = sin(phi1)*sin(phi2) + cos(phi1)*cos(phi2)*cos(theta);
    //return acos(cosAlpha);
    return inverseHaversine (haversine(phi1-phi2) + cos(phi1)*cos(phi2)*haversine(theta));
}

double Position::operator-(const Position& second) const
{
    return curve(second) * Global::EarthRadius;
}

void Position::move(double dlat, double dlon)
{
    latitude += dlat;
    longitude += dlon;
}

double Position::getAzimuth(const Position& second) const
{
    double cur = curve(second);
    
    double phi1 = latitude * M_PI / 180;
    double phi2 = second.latitude * M_PI / 180;
    
    double cosAzimuth = ( sin(phi2)-cos(cur)*sin(phi1)) / (sin(cur)*cos(phi1));
    
    if (cosAzimuth > 1.0)
        return 0.0;
    if (cosAzimuth < -1.0)
        return M_PI;
    // Now we should find azimuth, but we know only cos.
    Vector3d thisVec;
    createVecotr3D(thisVec);
    Vector3d secondVec;
    second.createVecotr3D(secondVec);
    Vector3d product;
    vectorProduct(product, thisVec, secondVec);
    if (product[2] < 0)
        return 2.0* M_PI-acos(cosAzimuth);
    
    return acos(cosAzimuth);
}

std::string Position::getString() const
{
    std::ostringstream oss;
    oss << std::setprecision (10)
        << latitude << "°" << getLatitudeChar() << "; "
        << longitude << "°" << getLongtitudeChar();
        
    return oss.str();
}

void Position::createVecotr3D(Vector3d vec) const
{
    double lat=latitude / 180 * M_PI;
    double lon=longitude / 180 * M_PI;
    vec[0] = cos(lon) * cos(lat);
    vec[1] = sin(lon) * cos(lat);
    vec[2] = sin(lat);
}

Position calculateCenterOfMass(const Position& pos1, double weight1,
                               const Position& pos2, double weight2)
{
    Vector3d v1;
    pos1.createVecotr3D(v1);
    Vector3d v2;
    pos2.createVecotr3D(v2);
    Vector3d originalZAxis = {0, 0, 1};
    
    Vector3d newZAxis;
    vectorProduct(newZAxis, v1, v2);
    normalize(newZAxis);
    
    Vector3d rotationAxis;
    vectorProduct(rotationAxis, originalZAxis, newZAxis);
    
    double rotCos = scalarProduct(originalZAxis, newZAxis);
    double rotSin = normal(rotationAxis);
    normalize(rotationAxis);
    
    Matrix3x3 rotMatrixTo;
    createRotationMatrix(rotMatrixTo, rotationAxis, rotCos, rotSin);
    
    Matrix3x3 rotMatrixFrom;
    createRotationMatrix(rotMatrixFrom, rotationAxis, rotCos, -rotSin);
    
    applyTransformation(rotMatrixFrom, v1);
    applyTransformation(rotMatrixFrom, v2);
    
    double angle1 = atan2(v1[1], v1[0]);
    double angle2 = atan2(v2[1], v2[0]);
    
    if (angle1 > angle2 && angle1-angle2 > M_PI) angle2 += 2*M_PI;
    if (angle1 < angle2 && angle2-angle1 > M_PI) angle1 += 2*M_PI;
    
    double resultAngle = (weight1*angle1 + weight2*angle2) / (weight1 + weight2);
    
    Vector3d resultVec = {cos(resultAngle), sin(resultAngle), 0};
    applyTransformation(rotMatrixTo, resultVec);
    normalize(resultVec);
    
    double longitude = atan2(resultVec[1], resultVec[0]) / M_PI * 180;
    double latitude = asin(resultVec[2]) / M_PI * 180;
    
    Position result(latitude, longitude);
    return result;
}
