/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "timestamp-utils.hpp"

#include <iomanip>
#include <boost/algorithm/string/replace.hpp>

using namespace std;

string getHumanReadableDayMonthYear(const StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss << setw(2) << setfill('0') << (unsigned int) timestampInfo.day << "."
        << setw(2) << setfill('0') << (unsigned int) timestampInfo.month << "."
        << timestampInfo.year;
    return oss.str();
}

string getHumanReadableTime(const StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss.precision(9 );
    oss << setw(2) <<setfill('0') << (unsigned int) timestampInfo.hours << ":"
        << setw(2) <<setfill('0') << (unsigned int) timestampInfo.minutes << ":"
        << setw(2) <<setfill('0') << (unsigned int) timestampInfo.seconds << "+"
        << ( (double) timestampInfo.TS_time) / ((double)timestampInfo.TS_Osc);
        
    return oss.str();
}

string getHumanReadableTimestamp(const StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss << getHumanReadableDayMonthYear(timestampInfo) << ", " << getHumanReadableTime(timestampInfo);
    return oss.str();
}

string getHumanReadableCoords(const StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss << timestampInfo.latitude_ns << timestampInfo.latitude_mas << "; "
        << timestampInfo.longitude_ew << timestampInfo.longitude_mas << ", "
        << "Height: " << timestampInfo.height_cm << "cm, " << endl
        << "DOP: " << timestampInfo.dop << " "
        << "Satellites: "
        << (unsigned int) timestampInfo.satellites_tracked << "/"
        << (unsigned int) timestampInfo.satellites_visible;
    return oss.str();
}

string getHumanReadableRawInfo(const StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss << "  TS_Osc: " << timestampInfo.TS_Osc << endl
    // TS_time is count of oscilator's oscilates from latest round second (from GPS)
        << "  TS_time: " << timestampInfo.TS_time << endl
    // TS_10ms is time from latest round second in 10ms intervals
        << "  TS_10ms: " << (unsigned int) timestampInfo.TS_10ms << endl
    // capture_time is not used and not initialised
    //    << ", capture_time: " << timestampInfo.capture_time
    
    // oscillator_temperature is always 0
        << "  oscillator_temperature: " << timestampInfo.oscillator_temperature << endl
    
        << "  serial_number: " << timestampInfo.serial_number << endl;
    return oss.str();
}

string formatFileNameWithTimestamp(const string &prefix, const string &suffix,
                                   StormProcess_tTIMESTAMPINFO& timestampInfo)
{
    ostringstream oss;
    oss << prefix
        << getHumanReadableTime(timestampInfo)
        << suffix;
    return oss.str();
}

std::string formatTime(const PosixTime& time)
{
    string result = to_simple_string(time);
    boost::algorithm::replace_first(result, "Jan", "01");
    boost::algorithm::replace_first(result, "Feb", "02");
    boost::algorithm::replace_first(result, "Mar", "03");
    boost::algorithm::replace_first(result, "Apr", "04");
    boost::algorithm::replace_first(result, "May", "05");
    boost::algorithm::replace_first(result, "Jun", "06");
    boost::algorithm::replace_first(result, "Jul", "07");
    boost::algorithm::replace_first(result, "Aug", "08");
    boost::algorithm::replace_first(result, "Sep", "09");
    boost::algorithm::replace_first(result, "Oct", "10");
    boost::algorithm::replace_first(result, "Nov", "11");
    boost::algorithm::replace_first(result, "Dec", "12");
    return result;
}

std::string formatTimeFS(const PosixTime& time)
{
	string result = formatTime(time);
	boost::algorithm::replace_all(result, " ", "_");
	boost::algorithm::replace_all(result, ":", "-");
	return result;
}
