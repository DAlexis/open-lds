#ifndef STRIKES_PROCESSOR_H_INCLUDED
#define STRIKES_PROCESSOR_H_INCLUDED

#include "positioning.hpp"
#include "strike-data.hpp"
#include "MysqlWrapper.hpp"
#include "logging.hpp"
#include "solver-queue.hpp"

#include <vector>
#include <string>
#include <memory>

#include <functional>

#include <mutex>
#include <thread>



class StrikesProcessor
{
public:
    StrikesProcessor();
    ~StrikesProcessor();
    void readConfig(const std::string& filename);
    void run(bool needCopyAndClasterisation = true, bool needSolving = true);
    
    void registerSignals();
    
private:
    
    void solve(size_t id, unsigned int solverNumber);
    bool enqueueNextCluster();
    bool clusterizingIteration();
    
    class ExQueryReturnedNothing {};
    class ExNoMinId : ExQueryReturnedNothing {};
    class ExNoMaxId : ExQueryReturnedNothing {};
    
    /// Load lastOperatedTimeClusterId as minimum id value (for the first start)
    size_t getMinTCId(MySQLConnectionManager& connection);
    size_t getMaxTCId(MySQLConnectionManager& connection);

    std::string simpleGMTQuery(const Time& from, const Time& to);
    bool sleepAndCheck(double time);
    
    static void staticSignalHandler(int signum);
    static StrikesProcessor* signalReceiverObject;
    void signalHandler(int signum);
    
    void runStrikesClusterisator();
    void runLightningsDetection();
    
    void stop();
    
    void restoreState();
    void saveState();
    
    void setDefaultState();
    
    bool sleepFixedDelay(Time& from, double delay);
    
    void createStrikeDataModifiers();
    
    std::vector<IStrikeDataModifier*> modifiers;
    std::string strikesDataModifiersString;
    
    std::mutex stateSavingMutex;
    
    
    MySQLConnectionManager clusteringSourceDb;
    MySQLConnectionManager clusteringDestinationDb;
    
    MySQLConnectionManager solvingSourceDb;
    MySQLConnectionManager solvingDestinationDb;
    
    Time lastClusterizedStrikeTime;
    
    Time lastSolverRun;
    size_t lastOperatedTimeClusterId;
    
    // Configuration variables
    double requestDurationMax, requestDurationMin;   /// Max time that can be processed by using one request to server
    double timeToWait;      /// Time to wait between requests
    double delayBetweenLastReadedAndRealtime; /// Minimal delay between last reading time and current time
    
    double timeZoneShift;   /// Current timezone (we assume than db use GMT)
    
    unsigned int solverWorkersCount; /// Count of threads that solving lightning positioning problem
    unsigned int maxQueueLength;     /// Max tasks count in a solver tasks queue
    unsigned int minQueueLength;     /// Tasks count after which loading continued
    
    
    bool needToStop;
    
    const std::string stateStrage;
    bool m_needCopyAndClasterisation, m_needSolving;
    
    size_t solverCrashesCount;
    size_t clusterizerCrashesCount;
    
    TasksQueue<size_t> solverQueue;
};

#endif // STRIKES_PROCESSOR_H_INCLUDED
