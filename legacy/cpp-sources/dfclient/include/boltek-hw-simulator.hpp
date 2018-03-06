/*
 * boltek-hw-simulator.hpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_BOLTEK_HW_SIMULATOR_HPP_
#define DFCLIENT_BOLTEK_HW_SIMULATOR_HPP_

#include "hardware-interface.hpp"
#include "positioning.hpp"

/**
 * This class simulates boltek hardware and generates test strikes data
 */
class BoltekHardwareSimulator : public ILightningDetectionHardware
{
public:
	BoltekHardwareSimulator() {}

	~BoltekHardwareSimulator() override;
	void initDevice() override;
	void setSquelch(double squelch) override;
	bool isStrikeReady() override;
	void closeDevice() override;
	std::shared_ptr<IStrikeDataContainer> getData() override;

private:
	constexpr static uint8_t triggerLevelMax = 15;

	void reportMissedStrikes();

	bool m_boardIsRunning = false;
	bool m_squelch = 1.0;

	double m_simulationPeriod = 1.0;
	Time m_lastStrikeTime;
};


class BoltekHardwareSimulatorFactory : public ILightningDetectionHardwareFactory
{
public:
	BoltekHardwareSimulatorFactory();
	ILightningDetectionHardware* create() override;
};

#endif /* DFCLIENT_BOLTEK_HW_SIMULATOR_HPP_ */
