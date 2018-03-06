#include "stats-calculator.hpp"
#include "configurator.hpp"
#include "exceptions.hpp"
#include <fstream>
#include <memory>

#include <signal.h>

namespace attrs = boost::log::attributes;

StatsCalculator* StatsCalculator::signalReceiver = nullptr;

void StatsCalculator::registerSignals()
{
    signal(SIGINT, &StatsCalculator::staticSignalHandler);
    signal(SIGTERM, &StatsCalculator::staticSignalHandler);
    signalReceiver = this;
}

void StatsCalculator::staticSignalHandler(int signum)
{
    BOOST_LOG_SEV(globalLogger, info) << "Received signal " << signum << "; stopping now";
    signalReceiver->stop();
}

void StatsCalculator::stop()
{
    m_stop = true;
}

void StatsCalculator::Stats::getSolutionsCount(MySQLConnectionManager& mysql)
{
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("solutions-counter"));
    m_solutionsCount = 0;
    
    std::string query = "SELECT COUNT(`id`) FROM `solutions` WHERE (round_time >= '"
        + m_time.getRoundPart() + "') && (round_time < '"
        + (m_time+60).getRoundPart() + "')";
    
    mysql.openConnection();
    
    if (mysql.query(query))
    {
        char* data = mysql.getData(0);
        if (data)
            m_solutionsCount = std::stoi(data);
    }
    
    mysql.closeConnection();
    BOOST_LOG_SEV(globalLogger, trace) << "Solutions found for " << m_time.getRoundPart() << ": " << m_solutionsCount;
}

void StatsCalculator::Stats::calculateStats(MySQLConnectionManager& mysql, Time time)
{
    m_time = time;
    getSolutionsCount(mysql);
}

void StatsCalculator::Stats::write(MySQLConnectionManager& mysql)
{
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("stats-writing"));
    MySQLInsertGenerator insert("strikes_stats");
    insert.set("round_time", m_time.getRoundPart(), true);
    insert.set("solutions_count", m_solutionsCount);
    
    mysql.openConnection();
    mysql << insert;
    mysql.closeConnection();
}

void StatsCalculator::restoreState()
{
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("StateRestorer"));
    BOOST_LOG_SEV(globalLogger, debug) << "Restoring state";
    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(stateFileName, pt);
        
        m_lastCalculationTime.setTime(pt.get<std::string>("state.last_operated_time"));
    }
    catch(boost::property_tree::ini_parser::ini_parser_error &exception)
    {
        // This means we cannot open configuration file with state. Not very bad thing
        BOOST_LOG_SEV(globalLogger, warning) << ".ini file parsing error in " + exception.filename() +  ":" + std::to_string(exception.line()) + " - " + exception.message();
        setDefaultState();
    }
    catch(boost::property_tree::ptree_error &exception)
    {
        // Hm... Parsing error - it is really strange
        BOOST_LOG_SEV(globalLogger, warning) << "Parsing error: " << exception.what();
        setDefaultState();
    }
    catch(...)
    {
        // Any other thing
        BOOST_LOG_SEV(globalLogger, warning) << "Undefined state config reading error";
    }
}


void StatsCalculator::saveState()
{
    using namespace std;
    unique_lock<mutex> lock(m_stateFileMutex);
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("StateSaver"));
    ofstream file(stateFileName);
    if (!file) {
        file.close();
        throw std::runtime_error(EX_PREFIX + "Cannot save state!");
    }
    try
    {
        file << "[state]" << endl;
        file << "last_operated_time = " << m_lastCalculationTime.getString() << endl;
        file.close();
        BOOST_LOG_SEV(globalLogger, debug) << "Deamon state saved";
    }
    catch(std::exception& ex)
    {
        throw std::runtime_error(EX_PREFIX + "Cannot save state! Error: " + ex.what());
    }
    catch(...)
    {
        throw std::runtime_error(EX_PREFIX + "Cannot save state! Unknown exception");
    }
}

void StatsCalculator::setDefaultState()
{
    m_lastCalculationTime.setTime("2014-05-01 00:00:00+0");
    BOOST_LOG_SEV(globalLogger, info) << "Setting state to default";
}


void StatsCalculator::run()
{
    m_database.hostName = StrikesStatsConfigurator::pt().get<std::string>("networking.hostname", "localhost");
    m_database.name     = StrikesStatsConfigurator::pt().get<std::string>("networking.database", "lightning_detection_system");
    m_database.userName = StrikesStatsConfigurator::pt().get<std::string>("networking.username", "root");
    m_database.password = StrikesStatsConfigurator::pt().get<std::string>("networking.password", "");
    
    m_delayBeforeNow = StrikesStatsConfigurator::pt().get<double>("timetable.delay_between_last_readed_and_realtime", 3600);
    
    restoreState();
    
    while (!m_stop)
    {
        std::unique_ptr<Stats> stats(new Stats);
        stats->calculateStats(m_database, m_lastCalculationTime);
        stats->write(m_database);
        m_lastCalculationTime += 60;
        while(Time() - m_lastCalculationTime < m_delayBeforeNow)
        {
            usleep(10000000);
        }
    }
    
    saveState();
}

