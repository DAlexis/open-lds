/*
 * health-monitor.hpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_HEALTH_MONITOR_HPP_
#define DFCLIENT_HEALTH_MONITOR_HPP_

#include "boost/date_time/posix_time/posix_time.hpp"
#include "macros.hpp"
#include "utils.hpp"
#include <string>
#include <map>
#include <thread>
#include <list>

class HealthMonitor : public LoopFuncHolder
{
public:

	SIGLETON_IN_CLASS(HealthMonitor)

	void init();

	void registerOutputProvider(const std::string& name);

	void outputProviderAddInitFail(const std::string& name);
	void outputProviderAddOutputFail(const std::string& name);
	void outputProviderAddFinalFail(const std::string& name);
	void outputProviderSetQueueSize(const std::string& name, size_t size);

	void addOtherMetric(const std::string& metricName);
	void setOtherMetric(const std::string& metricName, double value);
	void incOtherMetric(const std::string& metricName, double increment = 1.0);

	void setOtherStrMetric(const std::string& metricName, const std::string& value);

	void addStrike();
	void resetStats();

	std::string makeTextOutput();

	void stopAsync();

private:
	struct OutputProviderHealth
	{
		void reset() { *this = OutputProviderHealth(); }

		size_t queueSize = 0;
		size_t outputInitFails = 0;
		size_t outputFails = 0;
		size_t outputFinalizeFails = 0;
	};

	HealthMonitor();
	void loopFunction() override;

	class HealthMetrics
	{
	public:
		HealthMetrics();
		void reset();
		void doOutput();
		bool timeToOutput();

		std::map<std::string, OutputProviderHealth> outputProviders;
		size_t strikesCount;
		size_t period = 3600;
		std::string filenameTemplate;
		std::map<std::string, double> otherMetrics;
		std::map<std::string, std::string> otherStrMetrics;

	private:
		boost::posix_time::ptime m_lastOutputTime;
	};

	std::list<HealthMetrics> m_metrix;
};

#endif /* DFCLIENT_HEALTH_MONITOR_HPP_ */
