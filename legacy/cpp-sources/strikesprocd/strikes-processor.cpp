#include "strikes-processor.hpp"
#include "dfmath.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <functional>
#include <fstream>
#include <iostream>
#include <thread>
#include <memory>
#include <signal.h>

#define LAST_TC_ID_DEFAULT  0
#define MAX_CRASHES_COUNT   1000

using namespace std;
namespace attrs = boost::log::attributes;

StrikesProcessor* StrikesProcessor::signalReceiverObject = nullptr;

StrikesProcessor::StrikesProcessor() :
    // Configuration initialization here needed if readConfig() throw an exception and will not set default values
    requestDurationMax(3600),
    requestDurationMin(1000),
    timeToWait(5),
    delayBetweenLastReadedAndRealtime(3600),
    timeZoneShift(+4.0),
    solverWorkersCount(9),
    maxQueueLength(20),
    minQueueLength(9),
    needToStop(false),
    stateStrage("strikesprocd-state.conf"),
    m_needCopyAndClasterisation(true),
    m_needSolving(true),
    solverCrashesCount(0),
    clusterizerCrashesCount(0),
    solverQueue(std::bind(&StrikesProcessor::solve, this, std::placeholders::_1, std::placeholders::_2))
{
    lastClusterizedStrikeTime.setCurrent();
    lastSolverRun.setCurrent();
    lastOperatedTimeClusterId = LAST_TC_ID_DEFAULT;
}

StrikesProcessor::~StrikesProcessor()
{
    for (auto it = modifiers.begin(); it != modifiers.end(); it++)
        delete *it;
}


void StrikesProcessor::restoreState()
{
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("StateRestorer"));
    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(stateStrage, pt);
        
        lastClusterizedStrikeTime.setTime(pt.get<std::string>("state.last_lifecycle_time"));
        lastOperatedTimeClusterId = pt.get<size_t>("state.last_operated_time_cluster_id");
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

void StrikesProcessor::setDefaultState()
{
    lastClusterizedStrikeTime.setTime("2014-05-01 00:00:00+0");
    lastOperatedTimeClusterId = 0;
    BOOST_LOG_SEV(globalLogger, warning) << "Beginning from " << lastClusterizedStrikeTime.getString() << ", no time clusters operated before";
}

void StrikesProcessor::saveState()
{
    std::unique_lock<std::mutex> lock(stateSavingMutex);
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("StateSaver"));
    ofstream file(stateStrage);
    if (!file) {
        file.close();
        throw std::runtime_error(EX_PREFIX + "Cannot save state!");
    }
    try
    {
        file << "[state]" << endl;
        file << "last_lifecycle_time = " << lastClusterizedStrikeTime.getString() << endl;
        size_t lastCluster = solverQueue.empty() ? lastOperatedTimeClusterId : solverQueue.getNextTask()-1;
        file << "last_operated_time_cluster_id = " << lastCluster << endl << flush;
        file.close();
        BOOST_LOG_SEV(globalLogger, info) << "Deamon state saved";
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

void StrikesProcessor::readConfig(const std::string& filename)
{
    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(filename, pt);
        timeZoneShift   = pt.get<double>("global.time_zone", +4.0);
        
        solvingSourceDb.hostName   = clusteringSourceDb.hostName   = pt.get<std::string>("networking.source_hostname", "localhost");
        solvingSourceDb.name       = clusteringSourceDb.name       = pt.get<std::string>("networking.source_database", "lightning_detection_system");
        solvingSourceDb.userName   = clusteringSourceDb.userName   = pt.get<std::string>("networking.source_username", "root");
        solvingSourceDb.password   = clusteringSourceDb.password   = pt.get<std::string>("networking.source_password", "");
        
        solvingDestinationDb.hostName   = clusteringDestinationDb.hostName   = pt.get<std::string>("networking.destination_hostname", "localhost");
        solvingDestinationDb.name       = clusteringDestinationDb.name       = pt.get<std::string>("networking.destination_database", "lightning_detection_system");
        solvingDestinationDb.userName   = clusteringDestinationDb.userName   = pt.get<std::string>("networking.destination_username", "root");
        solvingDestinationDb.password   = clusteringDestinationDb.password   = pt.get<std::string>("networking.destination_password", "");
        
        
        timeToWait          = pt.get<double>("timetable.time_to_wait", 5);
        delayBetweenLastReadedAndRealtime = pt.get<double>("timetable.delay_between_last_readed_and_realtime", 3600);
        requestDurationMax  = pt.get<double>("timetable.request_duration_max", 3600);
        requestDurationMin  = pt.get<double>("timetable.request_duration_min", 3600);
        
        solverWorkersCount  = pt.get<unsigned int>("performance.solver_workers_count", 9);
        maxQueueLength      = pt.get<unsigned int>("performance.max_queue_length", 20);
        minQueueLength      = pt.get<unsigned int>("performance.min_queue_length", 9);
        
        strikesDataModifiersString = pt.get<std::string>("strike_preprocessing.modifiers", "");
    }
     
    catch(boost::property_tree::ini_parser::ini_parser_error &exception)
    {
        //std::ostringstream message
        throw std::logic_error(EX_PREFIX + ".ini file parsing error in " + exception.filename() +  ":" + std::to_string(exception.line()) + " - " + exception.message() );
    }
    catch(boost::property_tree::ptree_error &exception)
    {
        throw std::logic_error(EX_PREFIX + "Parsing error: " + exception.what());
    }
    catch(...)
    {
        throw std::logic_error(EX_PREFIX + "Undefined config reading error");
    }
    createStrikeDataModifiers();
}

void StrikesProcessor::runStrikesClusterisator()
{
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("Clusterizer"));
    BOOST_LOG_SEV(globalLogger, debug) << "StrikesProcessor::runStrikesClusterisator() started, last lifecycle ended " << lastClusterizedStrikeTime.getString();
    
    if (!m_needCopyAndClasterisation)
    {
        BOOST_LOG_SEV(globalLogger, info) << "Copying and clusterisation disabled by user";
        return;
    }
    
    for (;;)
    {
        try
        {
            if (!clusterizingIteration())
                break;
        }
        catch(std::exception& ex)
        {
            clusteringSourceDb.closeConnection();
            clusteringDestinationDb.closeConnection();
            clusterizerCrashesCount++;
            //if (clusterizerCrashesCount++ > MAX_CRASHES_COUNT) break;
            BOOST_LOG_SEV(globalLogger, error) << "Exception during clusterization: " << ex.what();
            if (sleepAndCheck(timeToWait))
                break;
        }
        catch(...)
        {
            clusteringSourceDb.closeConnection();
            clusteringDestinationDb.closeConnection();
            clusterizerCrashesCount++;
            //if (clusterizerCrashesCount++ > MAX_CRASHES_COUNT) break;
            BOOST_LOG_SEV(globalLogger, fatal) << "Unknown exception during clusterization!";
            if (sleepAndCheck(timeToWait))
                break;
        }
    }
    BOOST_LOG_SEV(globalLogger, info) << "Clusterizer's crashes count " << clusterizerCrashesCount;
}

bool StrikesProcessor::clusterizingIteration()
{
    if (needToStop) return false;
    
    // Waiting for some time
    Time nowTime;
    nowTime.setCurrent();
    
    StrikesGroup group;
    
    size_t readed = 0;
    
    Time timeForRequetsBegin = lastClusterizedStrikeTime;
    Time timeForRequestEnd = std::min(lastClusterizedStrikeTime + requestDurationMax, nowTime - delayBetweenLastReadedAndRealtime);
    
    if (timeForRequetsBegin > timeForRequestEnd)
    {
        BOOST_LOG_SEV(globalLogger, info) << "Last readed record was too close to current moment or in the future: " << lastClusterizedStrikeTime.getString() << ", so waiting.";
        return !sleepFixedDelay(nowTime, timeToWait);
    }
    
    // We should check if interval for request enough big
    double requestTime = timeForRequestEnd - timeForRequetsBegin;
    if (requestTime < requestDurationMin)
    {
        BOOST_LOG_SEV(globalLogger, debug) << "Request with time " << requestTime << "is too small (min allowed is " << requestDurationMin << ")";
        double timeDifference = requestDurationMin - (nowTime - delayBetweenLastReadedAndRealtime - timeForRequetsBegin);
        return !sleepFixedDelay(nowTime, std::min(timeToWait, timeDifference));
    }
    
    BOOST_LOG_SEV(globalLogger, trace) << "Opening connection to source db...";
    clusteringSourceDb.openConnection();
    BOOST_LOG_SEV(globalLogger, trace) << "done.";
    
    
    std::string query = simpleGMTQuery(timeForRequetsBegin, timeForRequestEnd);
    BOOST_LOG_SEV(globalLogger, info) << "Reading strikes records in time interval from " << timeForRequetsBegin.getString() << " to " << timeForRequestEnd.getString() << "...";
    readed += group.readWithCustomQuery(TT_BOLTEK_RAW, clusteringSourceDb, query);
    BOOST_LOG_SEV(globalLogger, debug) << "done, total readed " << readed;
    
    if (needToStop) return false;
    
    if (readed == 0)
    {
        lastClusterizedStrikeTime = timeForRequestEnd;
        clusteringSourceDb.closeConnection();
        BOOST_LOG_SEV(globalLogger, debug) << "There are 0 new records in base";
        return true;
    }
    
    BOOST_LOG_SEV(globalLogger, trace) << "Reading tail... ";
    readed += group.smartReadTail(TT_BOLTEK_RAW, clusteringSourceDb);
    BOOST_LOG_SEV(globalLogger, info) << "done, total readed " << readed;
    
    clusteringSourceDb.closeConnection();
    BOOST_LOG_SEV(globalLogger, trace) << "Connection closed.";
    
    if (needToStop) return false;
    
    BOOST_LOG_SEV(globalLogger, trace) << "Applying modifiers...";
    group.acceptModifiers(modifiers.begin(), modifiers.end());
    
    BOOST_LOG_SEV(globalLogger, trace) << "Detecting time clusters...";
    group.detectTimeClusters();
    BOOST_LOG_SEV(globalLogger, trace) << "done.";
    if (needToStop) return false;
    
    BOOST_LOG_SEV(globalLogger, trace) << "Opening connection to destination db...";
    clusteringDestinationDb.openConnection();
    //BOOST_LOG_SEV(globalLogger, debug) << "done.";
    
    clusteringDestinationDb << "SET autocommit=0";
    
    BOOST_LOG_SEV(globalLogger, trace) << "Writing strikes to destination db from cluster with more then one strike...";
    group.writeAllStrikesToDatabase(clusteringDestinationDb, true);
    //BOOST_LOG_SEV(globalLogger, debug) << "done.";
    
    BOOST_LOG_SEV(globalLogger, trace) << "Writing time clusters with more then one strike...";
    group.writeTimeClusters(clusteringDestinationDb);
    //BOOST_LOG_SEV(globalLogger, debug) << "done.";
    
    clusteringDestinationDb << "COMMIT";
    
    clusteringDestinationDb.closeConnection();
    
    lastClusterizedStrikeTime = group.getLatestStrikeTime() + boost::posix_time::hours(timeZoneShift);
    BOOST_LOG_SEV(globalLogger, debug) << "Lifecycle done, readed " << readed << " records";
    saveState();
    return true;
}

void StrikesProcessor::runLightningsDetection()
{
    BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >("SolverInputEnqueuer"));
    BOOST_LOG_SEV(globalLogger, debug) << "StrikesProcessor::runLightningsDetection() started";
    
    if (!m_needSolving)
    {
        BOOST_LOG_SEV(globalLogger, info) << "Solving disabled by user";
        return;
    }
    
    for (;;) {
        try
        {
            if (!enqueueNextCluster())
                break;
        }
        catch(std::exception& ex)
        {
            solvingSourceDb.closeConnection();
            solvingDestinationDb.closeConnection();
            solverCrashesCount++;
            //if (solverCrashesCount++ > MAX_CRASHES_COUNT) break;
            BOOST_LOG_SEV(globalLogger, error) << "Exception during solving: " << ex.what();
        }
        catch(...)
        {
            solvingSourceDb.closeConnection();
            solvingDestinationDb.closeConnection();
            solverCrashesCount++;
            //if (solverCrashesCount++ > MAX_CRASHES_COUNT) break;
            BOOST_LOG_SEV(globalLogger, fatal) << "Unknown exception during solving!";
        }
    }
    BOOST_LOG_SEV(globalLogger, info) << "Solver's crashes count " << solverCrashesCount;
}

void StrikesProcessor::solve(size_t id, unsigned int solverNumber)
{
    //BOOST_LOG_SCOPED_THREAD_ATTR("Job", attrs::constant< std::string >(std::string("Solver ") + std::to_string(solverNumber)));

 
    BOOST_LOG_SEV(globalLogger, info) << "Processing cluster with id = " << id;
    MySQLConnectionManager destinationDb(solvingDestinationDb);
    
    unique_ptr<StrikesGroup> pGroup(new StrikesGroup);
    destinationDb.openConnection();
    BOOST_LOG_SEV(globalLogger, trace) << "connection openned";
    try
    {
        BOOST_LOG_SEV(globalLogger, trace) << "reading begin";
        pGroup->readTimeCluster(destinationDb, id);
        BOOST_LOG_SEV(globalLogger, trace) << "reading done";
        destinationDb.closeConnection();
        BOOST_LOG_SEV(globalLogger, trace) << "closed";
        
    }
    catch (StrikesGroup::ExNoSuchTimeCluster& ex)
    {
        /// Hm. Cluster does not exists - removed manually?
        destinationDb.closeConnection();
        BOOST_LOG_SEV(globalLogger, info) << "Cluster with id " << id << " not found in database!";
        
        return;
    }
    
    BOOST_LOG_SEV(globalLogger, info) << "Solving for id " << id << "...";
    if (!pGroup->isEnoughData())
    {
        BOOST_LOG_SEV(globalLogger, info) << "id " << id << " has not enough data to solve it";
        return;
    }
    BOOST_LOG_SEV(globalLogger, trace) << "Creating solver";
    std::unique_ptr<IStrikesGroupSolver> solver(StrikesGroupSolverBuilder::instance().buildSolver("variational-2d"));
    BOOST_LOG_SEV(globalLogger, trace) << "Solver created";
    solver->solve(*pGroup);
    LightningPositioningResult& solution = solver->getMostProbableSolution();
    BOOST_LOG_SEV(globalLogger, trace) << "Solver created and solved";
    
    //LightningPositioningResult& solution = pGroup->getMostProbableSolution();
    
    destinationDb.openConnection();
    solution.writeSolution(destinationDb);
    destinationDb.closeConnection();
    
    BOOST_LOG_SEV(globalLogger, info)
        << "Solved for id " << id
        << " - time: " << solution.time.getString()
        << ", lat: " << solution.position.getLatitude()
        << ", lon: " << solution.position.getLongitude()
        << ".";
}

bool StrikesProcessor::enqueueNextCluster()
{
    if (needToStop) return false;
    // Code below can be optimized
    BOOST_LOG_SEV(globalLogger, trace) << "Opening connection";
    solvingDestinationDb.openConnection();
    BOOST_LOG_SEV(globalLogger, trace) << "Connection opened successfuly";
    size_t minIdInDB = 0, maxIdInDB = 0;
    
    try {
        minIdInDB = getMinTCId(solvingDestinationDb);
        maxIdInDB = getMaxTCId(solvingDestinationDb);
    }
    catch (ExNoMinId)
    {
        // May be table is empty.
        BOOST_LOG_SEV(globalLogger, info) << "Time clusters db seems empty";
        solvingDestinationDb.closeConnection();
        if (sleepFixedDelay(lastSolverRun, timeToWait)) return false;
        return true;
    }
    solvingDestinationDb.closeConnection();
    
    if (lastOperatedTimeClusterId == maxIdInDB)
    {
        BOOST_LOG_SEV(globalLogger, debug) << "Solutions are up-to-date";
        if (sleepFixedDelay(lastSolverRun, timeToWait)) return false;
        return true;
    }
    
    if (lastOperatedTimeClusterId+1 < minIdInDB)
    {
        BOOST_LOG_SEV(globalLogger, debug) << "Minimal id in db is " << minIdInDB << ", so we are going from beginning";
        lastOperatedTimeClusterId = minIdInDB-1;
    }
    
    // So we know id that was not processed yet. Lets put it into tasks queue.
    if (solverQueue.getQueueSize() > maxQueueLength) {
        BOOST_LOG_SEV(globalLogger, debug) << "Waiting queue...";
        solverQueue.waitForLength(minQueueLength);
    }
    solverQueue.addTask(++lastOperatedTimeClusterId);
    return true;
}

void StrikesProcessor::run(bool needCopyAndClasterisation, bool needSolving)
{
    m_needCopyAndClasterisation = needCopyAndClasterisation;
    m_needSolving = needSolving;
    
    solverQueue.init(solverWorkersCount);
    
    restoreState();
    thread clusterisationThread(std::bind(&StrikesProcessor::runStrikesClusterisator, this));
    thread detectionThread(std::bind(&StrikesProcessor::runLightningsDetection, this));
    
    clusterisationThread.join();
    detectionThread.join();
    stop();

    saveState();
}

void StrikesProcessor::signalHandler(int signum)
{
    switch(signum)
    {
        case SIGINT:
            BOOST_LOG_SEV(globalLogger, info) << "Signal " << signum << ": interrupting";
        break;
        case SIGTERM:
            BOOST_LOG_SEV(globalLogger, info) << "Signal " << signum << ": termination";
        break;
    }
    stop();
}

void StrikesProcessor::staticSignalHandler(int signum)
{
    signalReceiverObject->signalHandler(signum);
}

void StrikesProcessor::stop()
{
    needToStop = true;
    solverQueue.stop();
    //solverQueue.waitForStop();
    BOOST_LOG_SEV(globalLogger, info) << "Strikes proccessing tool stopped.";
}

void StrikesProcessor::registerSignals()
{
    StrikesProcessor::signalReceiverObject = this;
    
    signal(SIGINT, &StrikesProcessor::staticSignalHandler);
    signal(SIGTERM, &StrikesProcessor::staticSignalHandler);
}

bool StrikesProcessor::sleepAndCheck(double time)
{
    long int interval = 1e5; // Real sleeping during 0.1s
    for (double timeSlept = 0; timeSlept < time; timeSlept += interval)
    {
        if (needToStop) return true;
        usleep(interval);
    }
    return false;
}

std::string StrikesProcessor::simpleGMTQuery(const Time& from, const Time& to)
{
    std::ostringstream query;
    query << "(`when` > '"
          << (from - boost::posix_time::hours(timeZoneShift)).getRoundPart()
          << "') && "
          << "(`when` <= '"
          << (to - boost::posix_time::hours(timeZoneShift)).getRoundPart()
          << "')";
    return query.str();
}

size_t StrikesProcessor::getMinTCId(MySQLConnectionManager& connection)
{
    if (connection.query("SELECT min(`id`) FROM time_clusters WHERE 1"))
    {
        char* data = connection.getData(0);
        if (data)
            return std::stoi(data);
    }
    throw ExNoMinId();
}

size_t StrikesProcessor::getMaxTCId(MySQLConnectionManager& connection)
{
    if (connection.query("SELECT max(`id`) FROM time_clusters WHERE 1"))
    {
        char* data = connection.getData(0);
        if (data)
            return std::stoi(data);
    }
    throw ExNoMaxId();
}

bool StrikesProcessor::sleepFixedDelay(Time& from, double delay)
{
    Time nowTime;
    nowTime.setCurrent();
    double elapsedTime = nowTime - from;
    if (elapsedTime < delay)
        if (sleepAndCheck(1e6 * (timeToWait-elapsedTime))) return true;
    
    from.setCurrent();
    return false;
}

void StrikesProcessor::createStrikeDataModifiers()
{
    std::vector<std::string> modifierStrings;
    boost::split(modifierStrings, strikesDataModifiersString, boost::is_any_of(";"));
    for (auto it = modifierStrings.begin(); it != modifierStrings.end(); it++)
    {
        BOOST_LOG_SEV(globalLogger, info) << "Adding global data modifier: " << *it;
        modifiers.push_back(ModifiersGenerator().generateModifier(*it)); 
    }
}
