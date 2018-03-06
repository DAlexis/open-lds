/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "strike-data.hpp"
#include "table-index-mapping.hpp"
#include "stormpci.h"
#include "math-utils.hpp"
#include "constants.hpp"

#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

StrikeData::StrikeData() :
    uuid(boost::uuids::random_generator()()),
    m_deviceId(0),
    m_experimentId(0),
    m_serverDatabaseId(-1),
    bufferSize(0),
    bufferDuration(0),
    bufferBeginShift(0.0),
    directionCounted(false)
{
}

StrikeData::~StrikeData()
{
    
}

void StrikeData::read(TableType table, MySQLConnectionManager& mysql)
{
    switch(table)
    {
        case TT_UNIFIED: readUnifiedData(mysql); break;
        case TT_BOLTEK_RAW: readBoltekData(mysql); break;
        default: throw ExCannotLoadData();
    }
}

void StrikeData::read(TableType table, MySQLConnectionManager& mysql, int id)
{
    switch(table)
    {
        case TT_UNIFIED: readUnifiedData(mysql, id); break;
        case TT_BOLTEK_RAW: readBoltekData(mysql, id); break;
        default: throw ExCannotLoadData();
    }
}

void StrikeData::readBoltekData(MySQLConnectionManager& mysql)
{
    directionCounted = false;
    try
    {
        m_serverDatabaseId = std::stoi(mysql.getData(Boltek::RD_ID));
        m_deviceId = std::stoi(mysql.getData(Boltek::RD_DEVICE_ID));
        m_experimentId = std::stoi(mysql.getData(Boltek::RD_EXPERIMENT_ID));
        
        // Reading time
        double oscFreq = std::stod(mysql.getData(Boltek::RD_OSC_FREQ));
        double fractionalSecs = 0;
        
        /* This checking added because sometimes boltek returns zero as oscFreq
         * in this case we use oscFreq = 50 MHz
         */
        if (oscFreq > 1.0)
            fractionalSecs = std::stod(mysql.getData(Boltek::RD_COUNT_OSC)) / oscFreq;
        else
            fractionalSecs = std::stod(mysql.getData(Boltek::RD_COUNT_OSC)) / 5.0e7;
        
        time.setTime(mysql.getData(Boltek::RD_WHEN), fractionalSecs);
        
        // Reading coordinates
        position.setFromBoltekData(std::stod(mysql.getData(Boltek::RD_LAT)),
                              std::stod(mysql.getData(Boltek::RD_LON)),
                              mysql.getData(Boltek::RD_LAT_NS)[0],
                              mysql.getData(Boltek::RD_LON_EW)[0]);
        
        // Reading buffers
        unsigned int bufferSizeInBytes = BOLTEK_BUFFERSIZE*sizeof(int);
        
        if (mysql.getLength(Boltek::RD_E_FIELD) != bufferSizeInBytes
            || mysql.getLength(Boltek::RD_MN_FIELD) != bufferSizeInBytes
            || mysql.getLength(Boltek::RD_ME_FIELD) != bufferSizeInBytes)
            throw ExInvalidDataFormat();
        
        bufferSize = BOLTEK_BUFFERSIZE;
        
        int* rawBNField = new int[bufferSize];
        int* rawBEField = new int[bufferSize];
        int* rawEField = new int[bufferSize];
        
        BlobData<int>::extractArray(rawEField, mysql.getData(Boltek::RD_E_FIELD), bufferSize);
        BlobData<int>::extractArray(rawBNField, mysql.getData(Boltek::RD_MN_FIELD), bufferSize);
        BlobData<int>::extractArray(rawBEField, mysql.getData(Boltek::RD_ME_FIELD), bufferSize);
        
        // Boltek frequency is 8 MHz
        bufferDuration = double(BOLTEK_BUFFERSIZE) / (8.0e6);
        
        // Calculating real fields
        double averageBN = 125.0;
        double averageBE = 125.0;
        double averageE  = 0.5;
        bufferBeginShift = - bufferDuration / 2.0;
        
        BNField.reserve(bufferSize);
        BEField.reserve(bufferSize);
        EField.reserve(bufferSize);
        BField.reserve(bufferSize);
        
        averageBField = 0;
        for (unsigned int i=0; i < bufferSize; i++)
        {
            BNField.push_back(rawBNField[i] - averageBN);
            BEField.push_back(rawBEField[i] - averageBE);
            BField.push_back(sqrt(sqr(BNField[i]) + sqr(BEField[i])));
            EField.push_back(rawEField[i] - averageE);
            averageBField += BField[i];
        }
        averageBField /= bufferSize;
        delete[] rawBNField;
        delete[] rawBEField;
        delete[] rawEField;
    }
    catch(const std::exception &ex)
    {
        throw ExInvalidDataFormat();
    }
}

void StrikeData::readBoltekData(MySQLConnectionManager& mysql, int id)
{
    std::string query = "SELECT * FROM strikes WHERE global_id='" + std::to_string(id) + "'";
    if (not mysql.query(query))
        throw ExNoSuchStrike();
    
    readBoltekData(mysql);
}

void StrikeData::readUnifiedData(MySQLConnectionManager& mysql)
{
    directionCounted = false;
    try
    {
        // Reading ids
        m_serverDatabaseId = std::stoi(mysql.getData(Unified::SD_ID));
        m_deviceId = std::stoi(mysql.getData(Unified::SD_DEVICE_ID));
        m_experimentId = std::stoi(mysql.getData(Unified::SD_EXPERIMENT_ID));
        
        // Reading time     
        time.setTime(mysql.getData(Unified::SD_ROUNT_TIME),
                         std::stod(mysql.getData(Unified::SD_FRACTION_TIME)));
        
        // Reading coordinates
        position.setLatitude(std::stod(mysql.getData(Unified::SD_LATITUDE)));
        position.setLongitude(std::stod(mysql.getData(Unified::SD_LONGTITUDE)));
        
        // Reading buffer parameters
        bufferSize = std::stoi(mysql.getData(Unified::SD_BUFFER_VALUES_COUNT));
        bufferBeginShift = std::stod(mysql.getData(Unified::SD_BUFFER_BEGIN_TIME_SHIFT));
        bufferDuration = std::stod(mysql.getData(Unified::SD_BUFFER_DURATION));
        
        // Reading buffers
        unsigned int bufferSizeInBytes = bufferSize*sizeof(double);
        
        if (mysql.getLength(Unified::SD_E_FIELD) != bufferSizeInBytes
            || mysql.getLength(Unified::SD_MN_FIELD) != bufferSizeInBytes
            || mysql.getLength(Unified::SD_ME_FIELD) != bufferSizeInBytes)
            throw ExInvalidDataFormat();
        
        BNField.clear();
        BEField.clear();
        EField.clear();
        BField.clear();
        
        BNField.resize(bufferSize);
        BEField.resize(bufferSize);
        EField.resize(bufferSize);
        BField.resize(bufferSize);
        
        BlobData<double>::extractArray(EField.data(),  mysql.getData(Unified::SD_E_FIELD),  bufferSize);
        BlobData<double>::extractArray(BNField.data(), mysql.getData(Unified::SD_MN_FIELD), bufferSize);
        BlobData<double>::extractArray(BEField.data(), mysql.getData(Unified::SD_ME_FIELD), bufferSize);
        
        averageBField = 0;
        for (unsigned int i=0; i < bufferSize; i++)
        {
            BField[i] = sqrt(sqr(BNField[i]) + sqr(BEField[i]));
            averageBField += BField[i];
        }
        averageBField /= bufferSize;
    }
    catch(const std::exception &ex)
    {
        throw ExInvalidDataFormat();
    }
}

void StrikeData::readUnifiedData(MySQLConnectionManager& mysql, int id)
{
    std::string query = "SELECT * FROM unified_strikes WHERE id='" + std::to_string(id) + "'";
    if (not mysql.query(query))
        throw ExNoSuchStrike();
    
    readUnifiedData(mysql);
}

void StrikeData::throwOutOfRange(unsigned int index) const
{
    if (index >= bufferSize)
        throw ExOutOfRange();
}

double StrikeData::getE(size_t index) const
{
    throwOutOfRange(index);
    return EField[index];
}

double StrikeData::getBN(size_t index) const
{
    throwOutOfRange(index);
    return BNField[index];
}

double StrikeData::getBE(size_t index) const
{
    throwOutOfRange(index);
    return BEField[index];
}

double StrikeData::getB(size_t index) const
{
    throwOutOfRange(index);
    return BField[index];
}

double StrikeData::getEByTimeShift(double timeShift) const
{
    return getE((size_t) round((timeShift-bufferBeginShift)/bufferDuration*(bufferSize-1)));
}

double StrikeData::getBEByTimeShift(double timeShift) const
{
    return getBE((size_t) round((timeShift-bufferBeginShift)/bufferDuration*(bufferSize-1)));
}

double StrikeData::getBNByTimeShift(double timeShift) const
{
    return getBN((size_t) round((timeShift-bufferBeginShift)/bufferDuration*(bufferSize-1)));
}

double StrikeData::getBByTimeShift(double timeShift) const
{
    return getB((size_t) round((timeShift-bufferBeginShift)/bufferDuration*(bufferSize-1)));
}

std::vector<double>& StrikeData::getEVector()
{
    return EField;
}

std::vector<double>& StrikeData::getBEVector()
{
    return BEField;
}

std::vector<double>& StrikeData::getBNVector()
{
    return BNField;
}

std::vector<double>& StrikeData::getBVector()
{
    return BField;
}

unsigned int StrikeData::getBufferLength() const
{
    return bufferSize;
}

double StrikeData::getBufferDuration() const
{
    return bufferDuration;
}

double StrikeData::getTimeStep() const
{
    return bufferDuration / bufferSize;
}

double StrikeData::getBufferBeginTimeShift() const
{
    return bufferBeginShift;
}

double StrikeData::getBufferEndTimeShift() const
{
    return bufferBeginShift + bufferDuration;
}

Time StrikeData::getBufferBeginTime() const
{
    return time + bufferBeginShift;
}

Time StrikeData::getBufferEndTime() const
{
    return time + (bufferDuration + bufferBeginShift);
}

void StrikeData::setDeviceId(int id)
{
    m_deviceId = id;
}

double StrikeData::indexToTime(const unsigned int index) const
{
    throwOutOfRange(index);
    return index / (bufferSize - 1) * bufferDuration + bufferBeginShift;
}

unsigned int StrikeData::timeToIndex(const double time) const
{
    unsigned int result = (time - bufferBeginShift) / bufferDuration * (bufferSize-1);
    throwOutOfRange(result);
    return result;
}

double StrikeData::indexIntervalToTimeInterval(const unsigned int index) const
{
    return bufferDuration * index / bufferSize;
}

unsigned int StrikeData::timeIntervalToIndexInterval(const double time) const
{
    unsigned int result = ceil((time / bufferDuration) * bufferSize);
    return result;
}

boost::uuids::uuid& StrikeData::getUuid()
{
    return uuid;
}

bool StrikeData::operator>(const StrikeData& second) const
{
    return time > second.time;
}

bool StrikeData::operator<(const StrikeData& second) const
{
    return time < second.time;
}

bool StrikeData::operator==(const StrikeData& second) const
{
    return time == second.time;
}

void StrikeData::printToFiles(std::string filePrefix, std::string fields)
{
    std::vector<std::string> fieldsList;
    boost::split(fieldsList, fields, boost::is_any_of(",; "));
    
    for (auto it = fieldsList.begin(); it != fieldsList.end(); it++)
    {
        if (*it == "B") {
            std::ofstream file(filePrefix+"-B.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                file << indexToTime(i) << " " << BField[i] << std::endl;
            }
            file.close();
            continue;
        }
        if (*it == "BN") {
            std::ofstream file(filePrefix+"-BN.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                file << indexToTime(i) << " " << BNField[i] << std::endl;
            }
            file.close();
            continue;
        }
        if (*it == "BE") {
            std::ofstream file(filePrefix+"-BE.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                file << indexToTime(i) << " " << BEField[i] << std::endl;
            }
            file.close();
            continue;
        }
        if (*it == "E") {
            std::ofstream file(filePrefix+"-E.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                file << indexToTime(i) << " " << EField[i] << std::endl;
            }
            file.close();
            continue;
        }
        if (*it == "Dia") {
            std::ofstream file(filePrefix+"-Direction-diagram.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                file <<  BEField[i] << " " << BNField[i] << std::endl;
            }
            file.close();
            continue;
        }
        
        if (*it == "DiaPolar") {
            std::ofstream file(filePrefix+"-Direction-diagram-polarity.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                double E, N;
                if (EField[i] > 0) {
                    E = BEField[i];
                    N = BNField[i];
                } else {
                    E = -BEField[i];
                    N = -BNField[i];
                }
                file << E << " " << N << std::endl;
            }
            file.close();
            continue;
        }
        if (*it == "DiaWeight") {
            std::ofstream file(filePrefix+"-Direction-diagram-weight.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                double weight = sqr(BField[i]/averageBField);
                file << weight*BEField[i] << " " << weight*BNField[i] << std::endl;
            }
            file.close();
            continue;
        }
        if (*it == "DiaWeightPolar") {
            std::ofstream file(filePrefix+"-Direction-diagram-weight-polarity.txt");
            for (unsigned int i=0; i < bufferSize; i++)
            {
                double weight = sqr(BField[i]/averageBField);
                double E, N;
                if (EField[i] > 0) {
                    E = BEField[i];
                    N = BNField[i];
                } else {
                    E = -BEField[i];
                    N = -BNField[i];
                }
                file << weight*E << " " << weight*N << std::endl;
            }
            file.close();
            continue;
        }
        
    }
    
}

void StrikeData::writeUnifiedData(MySQLConnectionManager& mysql)
{
    std::ostringstream oss;

    BlobData<double> ef(EField.data(), bufferSize);
    BlobData<double> mnf(BNField.data(), bufferSize);
    BlobData<double> mef(BEField.data(), bufferSize);
    
    findDirection();
    
    MySQLInsertGenerator insert("unified_strikes");
    insert.set("device_id",                 m_deviceId);
    insert.set("experiment_id",             m_experimentId);
    insert.set("round_time",                time.getRoundPart(), true);
    insert.set("fraction_time",             time.getFractionPart());
    insert.set("buffer_duration",           bufferDuration);
    insert.set("buffer_values_count",       bufferSize);
    insert.set("buffer_begin_time_shift",   bufferBeginShift);
    insert.set("lon",                       position.getLongitude());
    insert.set("lat",                       position.getLatitude());
    insert.set("direction",                 getDirection().azimuth);
    insert.set("direction_error",           getDirection().errorRate);
    insert.set("E_field",                   ef.getString(), true);
    insert.set("MN_field",                  mnf.getString(), true);
    insert.set("MW_field",                  mef.getString(), true);
    
    mysql << insert;
    
    m_serverDatabaseId = mysql.last_insert_id();
    
    /*
    oss << std::setprecision (std::numeric_limits<double>::digits10 + 1)
        <<"INSERT INTO unified_strikes(device_id, "
        << "experiment_id, "
        << "round_time, "
        << "fraction_time, "
        << "buffer_duration, "
        << "buffer_values_count, "
        << "buffer_begin_time_shift, "
        << "lon, "
        << "lat, "
        << "direction, "
        << "direction_error, "
        << "E_field, "
        << "MN_field, "
        << "MW_field) "
        << "VALUES("
        << m_deviceId << ", "
        << m_experimentId << ", "
        << "'" << time.getRoundPart() << "', "
        << time.getFractionPart() << ", "
        << bufferDuration << ", "
        << bufferSize << ", "
        << bufferBeginShift << ", "
        << position.getLongitude() << ", "
        << position.getLatitude() << ", "
        << getDirection().azimuth << ", "
        << getDirection().errorRate << ", "
        << "'" << ef.getString() << "', "
        << "'" << mnf.getString() << "', "
        << "'" << mef.getString() << "')";

    mysql << oss.str();*/
}

const Direction& StrikeData::getDirection() const
{
    if (!directionCounted)
        throw std::logic_error(EX_PREFIX + "Direction was not found before request");
    return direction;
}

void StrikeData::findDirection()
{
    if (directionCounted)
        return;
    
    LinearRegression2DFinder finder;
    finder.setArrays(bufferSize, BEField.data(), BNField.data());
    direction.angle = finder.getAngle();
    direction.azimuth = angleTo14Quadrant(M_PI_2 - finder.getAngle());
    direction.errorRate = finder.getNormedError();
    directionCounted = true;
}

double StrikeData::calculateFiniteRadiusTerm(const Position& location) const
{
    double distance = location - position;
    const double smoothingRadius = Configuration::Instance().solving.finiteRadiusTermWeightSmoothingRadius; // meters
    if (distance < Configuration::Instance().boltek.maxDetectionDistance - smoothingRadius)
        return 0.0;
    
    if (distance > Configuration::Instance().boltek.maxDetectionDistance + smoothingRadius)
        return 1.0;
    
    return (1.0+sin((distance-Configuration::Instance().boltek.maxDetectionDistance) / smoothingRadius * M_PI_2)) / 2.0;
}

void StrikeData::moveBaseTime(double timeShift)
{
    time += timeShift;
    bufferBeginShift -= timeShift;
}

size_t StrikeData::getServerDatabaseId()
{
    return m_serverDatabaseId;
}

void StrikeData::acceptModifier(IStrikeDataModifier* modifier)
{
    modifier->modify(this);
}
