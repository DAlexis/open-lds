/*
 * hardware-interface.cpp
 *
 *  Created on: 29 марта 2016 г.
 *      Author: dalexies
 */

#include "hardware-interface.hpp"
#include "logging.hpp"

SINGLETON_IN_CPP(LDHwCreator)

LDHwCreator::LDHwCreator()
{
}

void LDHwCreator::registerFactory(const std::string& hwType, ILightningDetectionHardwareFactory* factory)
{
	auto it = m_factories.find(hwType);
	if (it != m_factories.end())
		throw std::runtime_error(std::string("Hardware interface factory ") + hwType + " already registered");

	m_factories[hwType] = factory;
}

ILightningDetectionHardware* LDHwCreator::create(const std::string& hwType)
{
	BOOST_LOG_FUNCTION();
	auto it = m_factories.find(hwType);
	if (it == m_factories.end())
		throw std::runtime_error(std::string("Hardware interface factory ") + hwType + " not registered");

	return it->second->create();
}

