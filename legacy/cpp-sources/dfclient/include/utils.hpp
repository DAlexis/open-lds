/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <thread>
#include <memory>
#include <functional>
#include <string>

class DeviceAndExperimentSpecifiedClass
{
public:
    void setExpDevIds(int experimentId, int deviceId);
    void setExpDevIds(DeviceAndExperimentSpecifiedClass *setFrom);
    
    DeviceAndExperimentSpecifiedClass();
    
protected:
    int m_deviceId;
    int m_experimentId;
};

/**
 * This class runs loopFunction() in a separate thread with a period
 * setted by setPeriod() call. 
 */
class LoopFuncHolder
{
public:

	using TaskStoppedCallback = std::function<void(void)>;

    LoopFuncHolder();
    virtual ~LoopFuncHolder();
    
    /// Start iterating of loopFunction()
    void startMainLoop(const std::string& name = std::string("unknown"));
    
    /// Safely stop iterating and close the thread
    void stopMainLoop(TaskStoppedCallback callback = nullptr);
    
    /// Wait until thread is done
    void join();

    void stopAndJoin(TaskStoppedCallback callback = nullptr);

    /// Set task period in seconds
    void setPeriod(double period);
    
protected:
    virtual void loopFunction() = 0;
    
    unsigned long int m_period;
    
    bool m_mainLoopIsStarted = false;
    bool m_mainLoopShouldBeCanceled = false;
    
private:
    void mainLoop();
    std::unique_ptr<std::thread> m_thread{nullptr};
    std::string m_name = "";
    TaskStoppedCallback m_callback = nullptr;
    bool m_warnIfNotJoinable = true;

};

#endif //UTILS_H_INCLUDED
