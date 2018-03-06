#ifndef SOLVER_QUEUE_H_INCLUDED
#define SOLVER_QUEUE_H_INCLUDED

#include "positioning.hpp"
#include "logging.hpp"

#include <vector>
#include <string>
#include <queue>
#include <memory>

#include <functional>

#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

namespace attrs = boost::log::attributes;

template <class TaskType>
class TasksQueue
{
public:
    using WorkerFucntionType = std::function<void(TaskType, unsigned int)>;
    
    TasksQueue(WorkerFucntionType workerFunction) : tasks(workerFunction) { }
    void init(unsigned int threadsCount) {
        for (unsigned int i=0; i<threadsCount; i++)
            solvers.push_back(WorkerThreadWrapper(tasks));
        for (auto it = solvers.begin(); it != solvers.end(); it++)
            it->run();
    }
    
    void setSleepingTime(double time);
    
    TaskType getNextTask() { return tasks.tasks.front(); }
    
    void addTask(TaskType task) { tasks.lock(); tasks.tasks.push(task); tasks.unlock(); }
    
    void waitForLength(unsigned int length) { tasks.lengthWeAreWaitingFor = length; tasks.waitLength(); }
    
    void stop() { tasks.needStop = true; tasks.lengthIsLesser.notify_all(); }
    
    size_t getQueueSize() { return tasks.tasks.size(); }
    
    void waitForStop() {
//        std::cout << "Call!" << std::endl;
  //      int i=0;
        for (auto it = solvers.begin(); it != solvers.end(); it++)
        {
    //        std::cout << "i=" << i++ << std::endl;
            it->waitForStop();
        }
    }
    
    bool empty() { return tasks.tasks.empty(); }
    
private:
    
    class QueueContext
    {
    public:
        QueueContext(WorkerFucntionType workerFunction) : lengthWeAreWaitingFor(2), needStop(false), workerFunction(workerFunction), sleepingTime(1e6) { }
        
        std::queue<TaskType> tasks;
        
        unsigned int lengthWeAreWaitingFor;
        
        bool needStop;
        
        void lock() { tasksMutex.lock(); }
        
        void unlock() { tasksMutex.unlock(); }
        
        void waitLength() {
            if (tasks.size() < lengthWeAreWaitingFor) return;
            std::unique_lock<std::mutex> lock(lesserThanMutex); lengthIsLesser.wait(lock);
        }
        
        void checkLength() {
            //std::unique_lock<std::mutex> lock(lesserThanMutex);
            if (tasks.size() < lengthWeAreWaitingFor)
                lengthIsLesser.notify_all();
        }
        
        WorkerFucntionType workerFunction;
        std::condition_variable lengthIsLesser;
        
        double sleepingTime;
    private:
    
        std::mutex tasksMutex;
        
        std::mutex lesserThanMutex;
        
        
    };
    
    class WorkerThreadWrapper
    {
    public:
        WorkerThreadWrapper(QueueContext& globalQueueContext)  : tasks(globalQueueContext) {
            static unsigned int solversCount = 0;
            solverNumber = solversCount++;            
            lastSolverRun.setCurrent();
        }
        
        void run() { workingThread.reset(new std::thread(&TasksQueue::WorkerThreadWrapper::mainLoop, this)); }
        
        void waitForStop() { std::cout << "join" << std::endl;
            workingThread->join(); 
            std::cout << "ok" << std::endl;}
        
        void mainLoop() {
            globalLogger.add_attribute("Worker", attrs::constant< unsigned int >(solverNumber));
            BOOST_LOG_SEV(globalLogger, trace) << "Starting worker";
            for(;;)
            {
                if (tasks.needStop) break;
                bool needWaiting = false;
                TaskType currentTask;
                
                tasks.lock();
                needWaiting = tasks.tasks.empty();
                if (!needWaiting) {
                    currentTask = tasks.tasks.front();
                    tasks.tasks.pop();
                    tasks.checkLength();
                }
                tasks.unlock();
                
                if (needWaiting) {
                    //BOOST_LOG_SEV(logger, trace) << "Empty queue";
                    if (sleepAndCheck(tasks.sleepingTime)) break;
                    continue;
                }
                
                tasks.workerFunction(currentTask, solverNumber);
            }
        }
        
    private:
        bool sleepAndCheck(double delay) {
            long int interval = 1e5; // Real sleeping during 0.1s
            for (double timeSlept = 0; timeSlept < delay; timeSlept += interval)
            {
                if (tasks.needStop) return true;
                usleep(interval);
            }
            return false;
        }
        
        std::unique_ptr<std::thread> workingThread;
        unsigned int solverNumber;
        QueueContext& tasks;
        Time lastSolverRun;
    };
    
    std::vector<WorkerThreadWrapper> solvers;
    
    QueueContext tasks;
    
    void threadLoop();
};

#endif // SOLVER_QUEUE_H_INCLUDED
