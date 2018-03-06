/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "data-collector.hpp"
#include "MysqlWrapper.hpp"
#include "logging.hpp"
#include "health-monitor.hpp"

#include <iostream>

DataCollector::DataCollector()
{

}

DataCollector::~DataCollector()
{
    stopMainLoop();
}

void DataCollector::collectData(std::shared_ptr<const IStrikeDataContainer> pstrike)
{
	for (auto it=m_outputContexts.begin(); it != m_outputContexts.end(); it++)
	{
		std::unique_lock<std::mutex> lock((*it)->queueMutex);
		(*it)->queue.push(pstrike);
	}
}

void DataCollector::registerOutputProvider(IOutputProvider* provider)
{
	BOOST_LOG_FUNCTION();
	BOOST_LOG_SEV(globalLogger, debug) << "Registering output provider " << provider->name();
	m_outputContexts.push_back(std::unique_ptr<OutputContext>(new OutputContext()));
	m_outputContexts.back()->provider.reset(provider);
	HealthMonitor::instance().registerOutputProvider(provider->name());
}

void DataCollector::loopFunction()
{
	BOOST_LOG_FUNCTION();
	for (auto it = m_outputContexts.begin(); it != m_outputContexts.end(); ++it)
	{
		OutputContext &context = *(*it);
		if (context.queue.empty())
			continue;

		BOOST_LOG_SEV(globalLogger, trace) << "Making output for " << (*it)->provider->name();

		HealthMonitor::instance().outputProviderSetQueueSize((*it)->provider->name(), context.queue.size());
		try
		{
			BOOST_LOG_NAMED_SCOPE("Group output begin try");
			context.provider->beginGroupOutput();
		}
		catch(std::exception& ex)
		{
			BOOST_LOG_SEV_EXTRACE(globalLogger, error, ex,
					"Error while initializing output to " << context.provider->name() << ": " << ex.what());
			HealthMonitor::instance().outputProviderAddInitFail(context.provider->name());
			continue;
		}
		catch(...)
		{
			BOOST_LOG_SEV(globalLogger, error)
					<< "Unknown exception while initializing output to " << context.provider->name();
			HealthMonitor::instance().outputProviderAddInitFail(context.provider->name());
			continue;
		}

		// Initializing output provider
		while (!context.queue.empty())
		{
			std::shared_ptr<const IStrikeDataContainer> data = context.queue.front();

			try
			{
				BOOST_LOG_NAMED_SCOPE("Output from queue");
				context.provider->doOutput(*data.get());
			}
			catch(std::exception& ex)
			{
				BOOST_LOG_SEV_EXTRACE(globalLogger, error, ex,
						"Error in process of output by " << context.provider->name() << ": " << ex.what());
				HealthMonitor::instance().outputProviderAddOutputFail(context.provider->name());
				break;
			}
			catch(...)
			{
				BOOST_LOG_SEV(globalLogger, error)
						<< "Error in process of output by " << context.provider->name() << ": unknown exception";
				HealthMonitor::instance().outputProviderAddOutputFail(context.provider->name());
				break;
			}

			// Locking is needed only for removing because this is only point that change front of queue
			std::unique_lock<std::mutex> lock(context.queueMutex);
			context.queue.pop();
		}

		try
		{
			BOOST_LOG_NAMED_SCOPE("Output finalizing");
			context.provider->endGroupOutput();
		}
		catch(std::exception& ex)
		{
			BOOST_LOG_SEV_EXTRACE(globalLogger, error, ex,
					"Error while finalizing output to " << context.provider->name() << ": " << ex.what());
			HealthMonitor::instance().outputProviderAddFinalFail(context.provider->name());
		}
		catch(...)
		{
			BOOST_LOG_SEV(globalLogger, error)
					<< "Unknown exception while finalizing output to " << context.provider->name();
			HealthMonitor::instance().outputProviderAddFinalFail(context.provider->name());
		}
	}
}

void DataCollector::init()
{
	setPeriod(Configurator::instance().get<double>("Output.period", 1.0));
    //outputManager.init();
}

void DataCollector::stopAsync()
{
	stopMainLoop();
}
