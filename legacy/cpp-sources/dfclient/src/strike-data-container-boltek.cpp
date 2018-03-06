/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "strike-data-container-boltek.hpp"

#include "MysqlWrapper.hpp"
#include "timestamp-utils.hpp"
#include <sstream>

using namespace std;

StrikeDataContainerBoltek::StrikeDataContainerBoltek()
{
	lightser::zerify(m_pboltekData);
}

StrikeDataContainerBoltek::StrikeDataContainerBoltek(const StormProcess_tBOARDDATA_t& newData)
{
    assign(newData);
}

StrikeDataContainerBoltek::~StrikeDataContainerBoltek()
{
}

void StrikeDataContainerBoltek::assign(const StormProcess_tBOARDDATA_t& newData)
{
    m_pboltekData = newData;
}

std::string StrikeDataContainerBoltek::getMySQLQuery() const
{
    BlobData<int> ef(m_pboltekData.EFieldBuf, BOLTEK_BUFFERSIZE);
    BlobData<int> mnf(m_pboltekData.NorthBuf, BOLTEK_BUFFERSIZE);
    BlobData<int> mef(m_pboltekData.EastBuf,  BOLTEK_BUFFERSIZE);
    
    MySQLInsertGenerator insert("strikes");
    insert.set("device_id", m_deviceId);
    insert.set("experiment_id", m_experimentId);
    insert.set("`when`", formatStrikeTime(m_pboltekData.lts2_data), true);
    insert.set("osc_freq", m_pboltekData.lts2_data.TS_Osc);
    insert.set("count_osc", m_pboltekData.lts2_data.TS_time);
    insert.set("lon", m_pboltekData.lts2_data.longitude_mas);
    insert.set("lat", m_pboltekData.lts2_data.latitude_mas);
    insert.set("lon_ew", m_pboltekData.lts2_data.longitude_ew, true);
    insert.set("lat_ns", m_pboltekData.lts2_data.latitude_ns, true);
    insert.set("E_field", ef.getString(), true);
    insert.set("MN_field", mnf.getString(), true);
    insert.set("MW_field", mef.getString(), true);
    return insert.generate();
}

void StrikeDataContainerBoltek::setDevExpId(int deviceId, int experimentId)
{
	m_deviceId = deviceId;
	m_experimentId = experimentId;
}

void StrikeDataContainerBoltek::serDeser(lightser::ByteStreamWrapper& bsw)
{
	bsw & m_pboltekData;
	bsw & m_deviceId;
	bsw & m_experimentId;
}

std::string StrikeDataContainerBoltek::formatStrikeTime(const StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss << timestampInfo.year << "-"
        << setw(2) << setfill('0') << (unsigned int) timestampInfo.month << "-"
        << setw(2) << setfill('0') << (unsigned int) timestampInfo.day << " "
        << setw(2) << setfill('0') << (unsigned int) timestampInfo.hours << ":"
        << setw(2) << setfill('0') << (unsigned int) timestampInfo.minutes << ":"
        << setw(2) << setfill('0') << (unsigned int) timestampInfo.seconds;
    return oss.str();
}

std::string StrikeDataContainerBoltek::getHumanReadableGPS(const StormProcess_tBOARDDATA& unpacked_info)
{
	std::string time = StrikeDataContainerBoltek::formatStrikeTime(unpacked_info.lts2_data);
	std::string position = std::to_string(unpacked_info.lts2_data.latitude_mas) + unpacked_info.lts2_data.latitude_ns + " "
				+ std::to_string(unpacked_info.lts2_data.longitude_mas) + unpacked_info.lts2_data.longitude_ew;

	return position + ", " + time;
}
