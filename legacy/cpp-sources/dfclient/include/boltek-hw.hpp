/*
 * boltek-hw.hpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_BOLTEK_HW_HPP_
#define DFCLIENT_BOLTEK_HW_HPP_

#include "hardware-interface.hpp"
#include <cstdint>

class BoltekHardwareInterface : public ILightningDetectionHardware
{
public:
	BoltekHardwareInterface() {}

	~BoltekHardwareInterface() override;
	void initDevice() override;
	void setSquelch(double squelch) override;
	bool isStrikeReady() override;
	void closeDevice() override;
	std::shared_ptr<IStrikeDataContainer> getData() override;

private:
	constexpr static uint8_t triggerLevelMax = 15;
	bool m_boardIsRunning = false;
	bool m_squelch = 1.0;
};

CONCRETE_FACTORY_HEADER(BoltekHardwareInterfaceFactory, ILightningDetectionHardware, ILightningDetectionHardwareFactory)

#endif /* DFCLIENT_BOLTEK_HW_HPP_ */
