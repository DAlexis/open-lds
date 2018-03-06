/*
 * hardware-interface.hpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_HARDWARE_INTERFACE_HPP_
#define DFCLIENT_HARDWARE_INTERFACE_HPP_

#include "strike-data-container-interface.hpp"
#include "macros.hpp"
#include <stdexcept>
#include <memory>
#include <map>

class ILightningDetectionHardware
{
public:
	virtual ~ILightningDetectionHardware() {}

	virtual void initDevice() = 0;
	virtual void setSquelch(double squelch) = 0;
	virtual bool isStrikeReady() = 0;
	virtual void closeDevice() = 0;
	virtual std::shared_ptr<IStrikeDataContainer> getData() = 0;

	WRAP_STDEXCEPT(ExCannotInitBoard, std::runtime_error)
};

ABSTRACT_FACTORY(ILightningDetectionHardwareFactory, ILightningDetectionHardware)

class LDHwCreator
{
public:
	void registerFactory(const std::string& hwType, ILightningDetectionHardwareFactory* factory);
	ILightningDetectionHardware* create(const std::string& hwType);

	SIGLETON_IN_CLASS(LDHwCreator)

private:
	LDHwCreator();
	std::map<const std::string, ILightningDetectionHardwareFactory*> m_factories;
};

#endif /* DFCLIENT_HARDWARE_INTERFACE_HPP_ */
