/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "receiver-controller.hpp"
#include "stormpci.h"
#include "exceptions.hpp"
#include "logging.hpp"
#include "health-monitor.hpp"

ReceiverController::ReceiverController(DataCollector& collector) :
    m_pDataCollector(&collector)
{

}

ReceiverController::~ReceiverController()
{
    stopMainLoop();
    closeDevice();
}

void ReceiverController::init()
{
	BOOST_LOG_FUNCTION();
	setExpDevIds(Configurator::instance().get<double>("Identification.experiment_id", 0),
				 Configurator::instance().get<double>("Identification.device_id", 0));
	setPeriod(Configurator::instance().get<double>("Receiver.sample_period", 0.01));

	const std::string hwType = Configurator::instance().get<std::string>("Receiver.hardware");
	BOOST_LOG_SEV(globalLogger, debug) << "Creating hardware controller for \"" << hwType << "\"...";
	m_lightningDetector.reset(
		LDHwCreator::instance().create(hwType)
	);
	BOOST_LOG_SEV(globalLogger, info) << "Initializing hardware controller for \"" << hwType << "\"...";

    Configurator::instance().get<double>("Receiver.sample_period", 0.01);
    
    m_lightningDetector->setSquelch(Configurator::instance().get<double>("Receiver.squelch", 1.0));

    m_lightningDetector->initDevice();
    BOOST_LOG_SEV(globalLogger, info) << "Initializing hardware done";
}

void ReceiverController::closeDevice()
{
	if (m_lightningDetector)
		m_lightningDetector->closeDevice();
}

void ReceiverController::stopAsync()
{
	stopMainLoop([this] { closeDevice(); } );
}

void ReceiverController::loopFunction()
{
	ASSERT(m_lightningDetector.get() != nullptr, std::runtime_error("Lightning detector hardware interface was not initialized"))
	BOOST_LOG_FUNCTION();

    if (!m_lightningDetector->isStrikeReady())
        return;
    
	BOOST_LOG_SEV(globalLogger, trace) << "Strike detected";
    std::shared_ptr<IStrikeDataContainer> pdata = m_lightningDetector->getData();
    HealthMonitor::instance().addStrike();
    
    pdata->setDevExpId(m_deviceId, m_experimentId);
    m_pDataCollector->collectData(pdata);
}
