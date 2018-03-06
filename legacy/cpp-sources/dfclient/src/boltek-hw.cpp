/*
 * boltek-hw.cpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#include "boltek-hw.hpp"
#include "exceptions.hpp"
#include "strike-data-container-boltek.hpp"
#include "health-monitor.hpp"

#include "stormpci.h"

CONCRETE_FACTORY_REGISTRATION(BoltekHardwareInterfaceFactory, LDHwCreator, "boltek");
CONCRETE_FACTORY_CREATE_METHOD(BoltekHardwareInterfaceFactory, BoltekHardwareInterface, ILightningDetectionHardware);


BoltekHardwareInterface::~BoltekHardwareInterface()
{
	closeDevice();
}

void BoltekHardwareInterface::initDevice()
{
	if (StormPCI_OpenPciCard() == 0)
	{
		throw ExCannotInitBoard("Cannot init PCI card: StormPCI_OpenPciCard() failed.");
	}
	setSquelch(m_squelch);
}

void BoltekHardwareInterface::setSquelch(double squelch)
{
	ASSERT(squelch >= 0.0 && squelch <= 1.0, std::logic_error("Squelch should be from [0.0, 1.0]"));
	m_squelch = squelch;
	uint8_t triggerLavel = (1.0 - m_squelch) * triggerLevelMax;
	if (m_boardIsRunning)
		StormPCI_SetSquelch(triggerLavel);
}

bool BoltekHardwareInterface::isStrikeReady()
{
	ASSERT(m_boardIsRunning, std::logic_error("isStrikeReady() call with Boltek PCI card is not opened"));
	return StormPCI_StrikeReady();
}

void BoltekHardwareInterface::closeDevice()
{
	if (m_boardIsRunning)
	{
		StormPCI_ClosePciCard();
		m_boardIsRunning = false;
	}
}

std::shared_ptr<IStrikeDataContainer> BoltekHardwareInterface::getData()
{
	ASSERT(m_boardIsRunning, std::logic_error("getData() call with Boltek PCI card is not opened"));

	if (!isStrikeReady())
		return nullptr;

	StormProcess_tPACKEDDATA packed_info;
	StormProcess_tBOARDDATA  unpacked_info;

	StormPCI_GetBoardData(&packed_info);
	StormPCI_RestartBoard();
	StormProcess_UnpackCaptureData(&packed_info, &unpacked_info);

	HealthMonitor::instance().setOtherStrMetric("GPS", StrikeDataContainerBoltek::getHumanReadableGPS(unpacked_info));

	std::shared_ptr<StrikeDataContainerBoltek> pdata(new StrikeDataContainerBoltek(unpacked_info));
	return pdata;
}
