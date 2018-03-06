#ifndef STATS_CALCULATOR_HPP_INCLUDED
#define STATS_CALCULATOR_HPP_INCLUDED

#include "MysqlWrapper.hpp"
#include "positioning.hpp"

#include "logging.hpp"

#include <mutex>

class StatsCalculator
{
public:
    void run();
    void registerSignals();
    
private:
    static void staticSignalHandler(int signum);
    
    class Stats
    {
    public:
        void calculateStats(MySQLConnectionManager& mysql, Time time);
        void write(MySQLConnectionManager& mysql);
        
    private:
        void getSolutionsCount(MySQLConnectionManager& mysql);
        Time m_time;
        size_t m_solutionsCount = 0;
    };
    
    
    
    void restoreState();
    void saveState();
    void setDefaultState();
    void stop();
    
    Time m_lastCalculationTime;
    double m_delayBeforeNow = 3600;
    MySQLConnectionManager m_database;
    bool m_stop = false;
    std::mutex m_stateFileMutex;
    
    
    const std::string stateFileName = "strikesstatd-state.conf";
    static StatsCalculator* signalReceiver;
};

#endif // STATS_CALCULATOR_HPP_INCLUDED
