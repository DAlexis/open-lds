/*
 * health-monitor.cpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#include "health-monitor.hpp"
#include "system-utils.hpp"
#include "configurator.hpp"
#include "timestamp-utils.hpp"
#include "logging.hpp"
#include "exceptions.hpp"

#include <fstream>
#include <algorithm>

SINGLETON_IN_CPP(HealthMonitor)


HealthMonitor::HealthMetrics::HealthMetrics()
{
	reset();
	m_lastOutputTime = boost::posix_time::second_clock::local_time();
}

void HealthMonitor::HealthMetrics::reset()
{
	strikesCount = 0;
	for (auto &it : outputProviders)
	{
		it.second.reset();
	}
	for (auto &it : otherMetrics)
	{
		it.second = 0;
	}
}

void HealthMonitor::HealthMetrics::doOutput()
{
	BOOST_LOG_FUNCTION();
	ASSERT(filenameTemplate != "", ATT_SCOPES_STACK(std::invalid_argument("Output filename is not set")));

	BOOST_LOG_SEV(globalLogger, debug) << "Producing health output to " << filenameTemplate;

	std::string result;
	boost::posix_time::ptime nowTime = boost::posix_time::second_clock::local_time();
	result += "[General]\n";
	result += "period_begin = " + formatTime(m_lastOutputTime) + "\n";
	result += "period_end = " + formatTime(nowTime) + "\n";
	result += "strikes_count = " + std::to_string(strikesCount) + "\n";
	result += "\n";
	for (auto &it : outputProviders)
	{
		result += "[" + it.first + "]\n";
		result += "queue_size = "            + std::to_string(it.second.queueSize) + "\n";
		result += "output_init_fails = "     + std::to_string(it.second.outputInitFails) + "\n";
		result += "output_fails = "          + std::to_string(it.second.outputFails) + "\n";
		result += "output_finalize_fails = " + std::to_string(it.second.outputFinalizeFails) + "\n";
		result += "\n";
	}

	result += "[Other]\n";
	for (auto &it : otherMetrics)
	{
		result += it.first + " = " + std::to_string(it.second) + "\n";
	}

	for (auto &it : otherStrMetrics)
	{
		result += it.first + " = " + it.second + "\n";
	}

	m_lastOutputTime = nowTime;

	const std::string tag = "%t";
	std::string resultFilename = SystemUtils::replaceTilta(filenameTemplate);
	size_t pos = resultFilename.find(tag);

	if (pos != result.npos)
		resultFilename.replace(pos, tag.length(), formatTimeFS(m_lastOutputTime));

	try {
		std::ofstream f(resultFilename, std::ofstream::out);
		f << result;
	}
	catch(std::exception& ex)
	{
		BOOST_LOG_SEV(globalLogger, error) << "Exception when writing to updatable file: " << ex.what();
	}
	catch(...)
	{
		BOOST_LOG_SEV(globalLogger, error) << "Exception when writing to updatable file: unknown exception";
	}

	reset();
}

bool HealthMonitor::HealthMetrics::timeToOutput()
{
	return (boost::posix_time::second_clock::local_time() - m_lastOutputTime).total_seconds() >= (long long int) period;
}

HealthMonitor::HealthMonitor()
{
}

void HealthMonitor::init()
{
	BOOST_LOG_FUNCTION();
	BOOST_LOG_SEV(globalLogger, info) << "Health monitor initialization";

	const std::string configPrefix = "Health-monitor";
	const boost::property_tree::ptree &pt = Configurator::instance().tree();

	for (auto it = pt.begin(); it != pt.end(); ++it)
	{
		if (std::mismatch(configPrefix.begin(), configPrefix.end(), it->first.begin()).first == configPrefix.end())
		{
			BOOST_LOG_SEV(globalLogger, trace) << "HM config instance";
			// We have instance of config
			std::string filename = it->second.get<std::string>("filename");
			size_t period = it->second.get<size_t>("output_update_period");

			m_metrix.push_back(HealthMetrics());
			m_metrix.back().period = period;
			m_metrix.back().filenameTemplate = filename;
		}
	}

	setPeriod(1.0);
}

void HealthMonitor::registerOutputProvider(const std::string& name)
{
	for (auto &it : m_metrix)
	{
		it.outputProviders[name] = OutputProviderHealth();
	}
}

void HealthMonitor::outputProviderAddInitFail(const std::string& name)
{
	for (auto &it : m_metrix)
	{
		it.outputProviders[name].outputInitFails++;
	}
}

void HealthMonitor::outputProviderAddOutputFail(const std::string& name)
{
	for (auto &it : m_metrix)
	{
		it.outputProviders[name].outputFails++;
	}
}

void HealthMonitor::outputProviderAddFinalFail(const std::string& name)
{
	for (auto &it : m_metrix)
	{
		it.outputProviders[name].outputFinalizeFails++;
	}
}

void HealthMonitor::outputProviderSetQueueSize(const std::string& name, size_t size)
{
	for (auto &it : m_metrix)
	{
		size_t &s = it.outputProviders[name].queueSize;
		s = std::max(s, size);
	}
}

void HealthMonitor::addOtherMetric(const std::string& metricName)
{
	setOtherMetric(metricName, 0.0);
}

void HealthMonitor::setOtherMetric(const std::string& metricName, double value)
{
	for (auto &it : m_metrix)
	{
		it.otherMetrics[metricName] = value;
	}
}

void HealthMonitor::incOtherMetric(const std::string& metricName, double increment)
{
	for (auto &it : m_metrix)
	{
		it.otherMetrics[metricName] += increment;
	}
}

void HealthMonitor::setOtherStrMetric(const std::string& metricName, const std::string& value)
{
	for (auto &it : m_metrix)
	{
		it.otherStrMetrics[metricName] = value;
	}
}

void HealthMonitor::addStrike()
{
	for (auto &it : m_metrix)
	{
		it.strikesCount++;
	}
}

void HealthMonitor::resetStats()
{
	for (auto &it : m_metrix)
	{
		it.reset();
	}
}

void HealthMonitor::loopFunction()
{
	BOOST_LOG_FUNCTION();
	for (auto &it : m_metrix)
	{
		if (it.timeToOutput())
		{
			it.doOutput();
		}
	}
}

void HealthMonitor::stopAsync()
{
	stopMainLoop();
}
