/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef STRIKE_DATA_CONTAINER_H_INLUDED
#define STRIKE_DATA_CONTAINER_H_INLUDED

#include "strike-data-container-interface.hpp"
#include "stormpci.h"
#include "utils.hpp"
#include "serialization.hpp"

#include <string>
#include <memory>

/// This class store strike data for every kind of device and form MySQL query for data
class StrikeDataContainerBoltek : public IStrikeDataContainer, public lightser::IBSWFriendly
{
public:
    StrikeDataContainerBoltek();
    StrikeDataContainerBoltek(const StormProcess_tBOARDDATA_t& newData);
    ~StrikeDataContainerBoltek();
    
    /// Keep StrikeDataContainer to hold boltek data. Will be overloaded for another type of DF
    void assign(const StormProcess_tBOARDDATA_t& newData);
    /// Get MySQL query to insert this strike
    std::string getMySQLQuery() const override;
    void setDevExpId(int deviceId, int experimentId) override;
    void serDeser(lightser::ByteStreamWrapper& bsw) override;

    static std::string formatStrikeTime(const StormProcess_tTIMESTAMPINFO& timestampInfo);
    static std::string getHumanReadableGPS(const StormProcess_tBOARDDATA& unpacked_info);

private:
    StormProcess_tBOARDDATA_t m_pboltekData;

    int m_deviceId = 0;
	int m_experimentId = 0;
};

#endif
