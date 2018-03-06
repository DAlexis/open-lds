/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "utils.hpp"
#include "logging.hpp"

#include <iostream>
#include <unistd.h>
#include <chrono>

DeviceAndExperimentSpecifiedClass::DeviceAndExperimentSpecifiedClass() :
    m_deviceId(0),
    m_experimentId(0)
{
}

void DeviceAndExperimentSpecifiedClass::setExpDevIds(int experimentId, int deviceId)
{
    m_deviceId = deviceId;
    m_experimentId = experimentId;
}

void DeviceAndExperimentSpecifiedClass::setExpDevIds(DeviceAndExperimentSpecifiedClass *setFrom)
{
    m_deviceId = setFrom->m_deviceId;
    m_experimentId = setFrom->m_experimentId;
}

LoopFuncHolder::LoopFuncHolder()
{
	setPeriod(1.0);
}

LoopFuncHolder::~LoopFuncHolder()
{
	BOOST_LOG_FUNCTION();
	if (!m_mainLoopIsStarted)
		return;
    stopMainLoop();
    join();
}

void LoopFuncHolder::setPeriod(double period)
{
    m_period = period * 1e3;
}

void LoopFuncHolder::startMainLoop(const std::string& name)
{
	BOOST_LOG_FUNCTION();
	m_name = name;
	BOOST_LOG_SEV(globalLogger, debug) << "Starting main loop for task " << m_name;
    if (m_mainLoopIsStarted)
    {
        throw std::runtime_error("Cycled task already started");
    }
    m_mainLoopShouldBeCanceled = false;
    m_mainLoopIsStarted = true;
    m_warnIfNotJoinable = true;
    m_thread.reset(new std::thread([this]{ mainLoop(); }));
}

void LoopFuncHolder::stopMainLoop(TaskStoppedCallback callback)
{
	BOOST_LOG_FUNCTION();
    if (not m_mainLoopIsStarted)
    {
    	//BOOST_LOG_SEV(globalLogger, trace) << "Task " << m_name << " not running, cant stop";
        return;
    }
    
    m_callback = callback;
    m_mainLoopShouldBeCanceled = true;
}

void LoopFuncHolder::join()
{
	BOOST_LOG_FUNCTION();
	if (m_thread == nullptr)
	{
		BOOST_LOG_SEV(globalLogger, warning) << "Task " << m_name << " not created, can't join";
		return;
	}

	if (m_thread->joinable())
	{
		m_thread->join();
		m_warnIfNotJoinable = false; // If we joined yet we should not warn
	} else {
		if (m_warnIfNotJoinable)
		{
			BOOST_LOG_SEV(globalLogger, warning) << "Task " << m_name << "'s thread is not joinable";
		}
	}
}

void LoopFuncHolder::stopAndJoin(TaskStoppedCallback callback)
{
	stopMainLoop(callback);
	join();
}


void LoopFuncHolder::mainLoop()
{
	BOOST_LOG_FUNCTION();
    while (!m_mainLoopShouldBeCanceled)
    {
        loopFunction();
        std::this_thread::sleep_for(std::chrono::milliseconds(m_period));
    }
    BOOST_LOG_SEV(globalLogger, trace) << "Task " << m_name << "'s thread is done";
    if (m_callback)
    	m_callback();

    //m_mainLoopIsStarted = false;
    m_mainLoopShouldBeCanceled = false;
}
