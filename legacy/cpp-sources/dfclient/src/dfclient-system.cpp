/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "dfclient-system.hpp"
#include "output-provider-interface.hpp"
#include "health-monitor.hpp"
#include "system-utils.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

DFClientSystem::DFClientSystem():
    receiver(collector)
{
    
}

DFClientSystem::~DFClientSystem()
{
}

void DFClientSystem::setupLogger()
{
	std::string logFileDir = SystemUtils::homeDir() + "/dfclient";
	if (!boost::filesystem::exists(logFileDir))
	{
		boost::filesystem::path dir(logFileDir);
		boost::filesystem::create_directory(dir);
	}

	initLoging(logFileDir + "/dfclient", info, trace);
}


void DFClientSystem::init(const std::string& configFileName)
{
	BOOST_LOG_FUNCTION();
	BOOST_LOG_SEV(globalLogger, info) << "dfclient system initialization";
    try
    {
    	BOOST_LOG_NAMED_SCOPE("Reading config");
        Configurator::instance().readConfig(configFileName);

        HealthMonitor::instance().init();
        collector.init();

        std::string outputProviders = Configurator::instance().get<std::string>("Output.providers");
        std::vector<std::string> providers;
        boost::split(providers, outputProviders, boost::is_any_of(",; "), boost::token_compress_on);
        for (auto it = providers.begin(); it != providers.end(); it++)
        {
        	IOutputProvider* provider = OutputProviderCreator::instance().create(*it);
        	provider->init("Output-provider-" + *it);
        	collector.registerOutputProvider(provider);
        }

        receiver.init();
    }
    catch(const Configurator::ExConfiguratorFailed &ex)
    {
    	BOOST_LOG_SEV_EXTRACE(globalLogger, fatal, ex, "Config reading error: " << ex.what());
        throw ATT_SCOPES_STACK(ExInitFailed("System initialization failed"));
    }
    catch(const std::exception &ex)
    {
    	BOOST_LOG_SEV_EXTRACE(globalLogger, fatal, ex, "System initialization failed: " << ex.what());
        throw ATT_SCOPES_STACK(ExInitFailed("System initialization failed"));
    }
    catch(...)
    {
    	BOOST_LOG_SEV(globalLogger, fatal) << "System initialization failed: unknown exception";
    	throw ATT_SCOPES_STACK(ExInitFailed("System initialization failed"));
    }

    //if all is good,
    BOOST_LOG_SEV(globalLogger, info) << "Starting main loops for system components";
    try {
    	BOOST_LOG_NAMED_SCOPE("Starting main loop");

    	receiver.startMainLoop("receiver");
		collector.startMainLoop("collector");
		HealthMonitor::instance().startMainLoop("health_monitor");
    }
    catch(const std::exception &ex)
    {
    	BOOST_LOG_SEV_EXTRACE(globalLogger, fatal, ex, "System crashed during starting main loop: " << ex.what());
    	throw ATT_SCOPES_STACK(ExRuntimeError("System runtime error"));
    }
    catch(...)
    {
    	BOOST_LOG_SEV(globalLogger, fatal) << "System crashed during starting loop: unknown exception";
    	throw ATT_SCOPES_STACK(ExRuntimeError("System runtime error"));
    }
}

void DFClientSystem::stop()
{
	BOOST_LOG_FUNCTION();

	receiver.stopAsync();
	collector.stopAsync();
	HealthMonitor::instance().stopAsync();
}

void DFClientSystem::join()
{
	HealthMonitor::instance().join();
	collector.join();
	receiver.join();
}
