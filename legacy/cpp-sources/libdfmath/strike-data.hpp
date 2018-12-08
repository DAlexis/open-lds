/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef UNIFIED_STRIKE_DATA_H_INCLUDED
#define UNIFIED_STRIKE_DATA_H_INCLUDED

#include "table-index-mapping.hpp"
#include "MysqlWrapper.hpp"
#include "stormpci.h"
#include "timestamp-utils.hpp"
#include "positioning.hpp"

#include <boost/shared_array.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

struct Direction
{
    /** @brief Means azimuth like on compass (counted from Oy with positive direction CV), BUT interval is [-pi/2; pi/2]
     */
    double azimuth = 0.0;
    /** @brief Result of linear regression: 
     * cos(angle)x+sin(angle)y+d=0
     * angle in [-pi/2;pi/2] counted from Ox axis eith positive direction CCV
     */
    double angle = 0.0;
    /** @brief Some kind of relative error factor. More value of errorRate -- more error
     */
    double errorRate = 0.0;
};

class StrikeData;

/// Visitor for strike data. Why visitor? - why not? Seriously, it may be useful later, but i'm not shure.
class IStrikeDataModifier
{
public:
    virtual ~IStrikeDataModifier() {}
    virtual void modify(StrikeData* strike) = 0;
};

class StrikeData
{
public:
    StrikeData();
    ~StrikeData();
    
    /** @brief Read data from any table when query is done and table rows are ready to get them
     * @param table Type of table
     */
    void read(TableType table, MySQLConnectionManager& mysql);
    void read(TableType table, MySQLConnectionManager& mysql, int id);
    
    /// Write unified data to special db
    void writeUnifiedData(MySQLConnectionManager& mysql);
    
    /// Get Direction structure with actual angle and dispersion
    const Direction& getDirection() const;
    
    void findDirection();
    
    /// Returns 1.0 when test point is too far from station and zero if close. Smoothing radius is in configuration
    double calculateFiniteRadiusTerm(const Position& location) const;
    
    /// Change time and bufferBeginShift according to timeShift
    void moveBaseTime(double timeShift);
    
    /// Accept data modifier
    void acceptModifier(IStrikeDataModifier* modifier);
    
    // Information
    unsigned int getBufferLength() const;
    double getBufferDuration() const;
    double getTimeStep() const;
    double getBufferBeginTimeShift() const;
    double getBufferEndTimeShift() const;
    Time getBufferBeginTime() const;
    Time getBufferEndTime() const;
    
    // Values
    double getE(size_t index) const;
    double getBE(size_t index) const;
    double getBN(size_t index) const;
    double getB(size_t index) const;
    
    double getEByTimeShift(double timeShift) const;
    double getBEByTimeShift(double timeShift) const;
    double getBNByTimeShift(double timeShift) const;
    double getBByTimeShift(double timeShift) const;
    
    std::vector<double>& getEVector();
    std::vector<double>& getBEVector();
    std::vector<double>& getBNVector();
    std::vector<double>& getBVector();
    
    // Setting
    void setDeviceId(int id);
    
    // Interval conversions
    double indexToTime(const unsigned int index) const;
    unsigned int timeToIndex(const double time) const;
    
    double indexIntervalToTimeInterval(unsigned int index) const;
    unsigned int timeIntervalToIndexInterval(double time) const;
    
    // Output
    void printToFiles(std::string filePrefix, std::string fields);
    
    size_t getServerDatabaseId();
    
    Position position;
    Time time;
    
    //////////////////
    // Exceptions 
    class Exception : public std::exception {};
    
    class ExOutOfRange : public Exception
    {
    public:
        virtual const char* what() const throw() { return "Value or index is out of range"; }
    };
    
    class ExCannotLoadData : public Exception {};
    
    class ExCannotAccessToMySQL : public ExCannotLoadData
    {
    public:        
        virtual const char* what() const throw() { return "Cannot load strike data from MySQL"; }
    };
    
    class ExCannotWriteToMySQL : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Cannot write strike data to MySQL"; }
    };
    
    class ExNoSuchStrike : public ExCannotLoadData
    {
    public:        
        virtual const char* what() const throw() { return "No strike with specified id in MySQL base"; }
    };
    
    class ExInvalidDataFormat : public ExCannotLoadData
    {
    public:        
        virtual const char* what() const throw() { return "Cannot understand data from table"; }
    };
    
    boost::uuids::uuid& getUuid();
    
    /// Compare times
    bool operator>(const StrikeData& second) const;
    bool operator<(const StrikeData& second) const;
    bool operator==(const StrikeData& second) const;
    
private:
    /// Read data from db with boltek only strikes when query is already done
    void readBoltekData(MySQLConnectionManager& mysql);
    /// Complete query and read data from db with boltek only strikes
    void readBoltekData(MySQLConnectionManager& mysql, int id);
    
    /// Read data from main db with data in unified format when uery is already done
    void readUnifiedData(MySQLConnectionManager& mysql);
    /// Complete query and read from main db with data in unified format
    void readUnifiedData(MySQLConnectionManager& mysql, int id);
    /// Unique identifier for this strike in memory
    boost::uuids::uuid uuid;
    
    /// Check index to be out of range and throw exception if it is
    void throwOutOfRange(unsigned int index) const;
    
    int m_deviceId;
    int m_experimentId;
    
    size_t m_serverDatabaseId = 0;
    
    unsigned int bufferSize;
    double bufferDuration;    
    double bufferBeginShift;
    
    /// @todo [low] put all fields to one sample structure?
    std::vector<double> BNField;
    std::vector<double> BEField;
    std::vector<double> BField;
    std::vector<double> EField;    
    
    double averageBField;
    
    bool directionCounted;
    Direction direction;
};

#endif //UNIFIED_STRIKE_DATA_H_INCLUDED
