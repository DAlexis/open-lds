/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef _TIMESTAMP_UTILS_H_
#define _TIMESTAMP_UTILS_H_

#include "stormpci.h"
#include <sstream>
#include <string>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

typedef boost::posix_time::ptime PosixTime;
/*
std::string getHumanReadableDayMonthYear(const StormProcess_tTIMESTAMPINFO& timestampInfo);
std::string getHumanReadableTime(const StormProcess_tTIMESTAMPINFO& timestampInfo);
std::string getHumanReadableTimestamp(const StormProcess_tTIMESTAMPINFO& timestampInfo);
std::string getHumanReadableCoords(const StormProcess_tTIMESTAMPINFO& timestampInfo);
std::string getHumanReadableRawInfo(const StormProcess_tTIMESTAMPINFO& timestampInfo);
std::string formatFileNameWithTimestamp(const std::string &prefix, const std::string &suffix,
                                   StormProcess_tTIMESTAMPINFO& timestampInfo);
std::string formatStrikeTime(const StormProcess_tTIMESTAMPINFO& timestampInfo);
*/
/// Create date and time string that acceptable by MySQL 
std::string formatTime(const PosixTime& time);

/// Create date and time string that acceptable by all file systems
std::string formatTimeFS(const PosixTime& time);

template <class T>
void outputArray(std::string fileName, T* array)
{
    std::ofstream file(fileName);
    for (int i = 0; i<BOLTEK_BUFFERSIZE; i++)
    {
        file << array[i] << std::endl;
    }
    file.close();
}

#endif //_TIMESTAMP_UTILS_H_
