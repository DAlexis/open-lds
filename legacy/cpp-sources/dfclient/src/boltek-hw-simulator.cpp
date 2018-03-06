/*
 * boltek-hw-simulator.cpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#include "boltek-hw-simulator.hpp"
#include "health-monitor.hpp"
#include "configurator.hpp"
#include "logging.hpp"
#include "strike-data-container-boltek.hpp"
#include <cstring>

BoltekHardwareSimulatorFactory factory;

BoltekHardwareSimulatorFactory::BoltekHardwareSimulatorFactory()
{
	LDHwCreator::instance().registerFactory("boltek-simulator", this);
}

ILightningDetectionHardware* BoltekHardwareSimulatorFactory::create()
{
	return new BoltekHardwareSimulator();
}


BoltekHardwareSimulator::~BoltekHardwareSimulator()
{
}

void BoltekHardwareSimulator::initDevice()
{
	HealthMonitor::instance().addOtherMetric("boltek-simulator-missed-strikes");
	m_simulationPeriod = Configurator::instance().get<double>("Boltek-simulator.period", m_simulationPeriod);
}

void BoltekHardwareSimulator::setSquelch(double squelch)
{
}

bool BoltekHardwareSimulator::isStrikeReady()
{
	Time nowTime;
	nowTime.setCurrent();
	return nowTime - m_lastStrikeTime > m_simulationPeriod;
}

void BoltekHardwareSimulator::closeDevice()
{

}

std::shared_ptr<IStrikeDataContainer> BoltekHardwareSimulator::getData()
{
	if (!isStrikeReady())
		return nullptr;

	reportMissedStrikes();
	StormProcess_tBOARDDATA  unpacked_info;

	memset(&unpacked_info, 0, sizeof(unpacked_info));
	unpacked_info.lts2_data.longitude_ew = 'E';
	unpacked_info.lts2_data.latitude_ns = 'N';
	m_lastStrikeTime.setCurrent();

	HealthMonitor::instance().setOtherStrMetric("GPS", StrikeDataContainerBoltek::getHumanReadableGPS(unpacked_info));

	return std::make_shared<StrikeDataContainerBoltek>(unpacked_info);
}

void BoltekHardwareSimulator::reportMissedStrikes()
{
	Time nowTime;
	size_t count = (nowTime - m_lastStrikeTime) / m_simulationPeriod - 1;
	if (count >= 1)
		HealthMonitor::instance().incOtherMetric("boltek-simulator-missed-strikes", count-1);
}
