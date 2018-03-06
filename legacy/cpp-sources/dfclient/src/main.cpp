/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "dfclient-system.hpp"
#include "logging.hpp"
#include <iostream>
#include <signal.h>
#include <memory>

#include <chrono>

std::unique_ptr<DFClientSystem> dfclient;

void signalCallbackHandler(int signum)
{
	BOOST_LOG_FUNCTION();

    switch(signum)
    {
        case SIGINT:
        	BOOST_LOG_SEV(globalLogger, info) << "Signal " << signum << ": interrupted by user.";
        break;
        case SIGTERM:
        	BOOST_LOG_SEV(globalLogger, info) << "Signal " << signum << ": termination.";
        break;
    }
    dfclient->stop();
    BOOST_LOG_SEV(globalLogger, trace) << "dfclient system stopped";
}

int main()
{
	DFClientSystem::setupLogger();
	BOOST_LOG_FUNCTION();

	//logging::core::get()->add_global_attribute("Scope", boost::make_shared< attrs::named_scope >());

	BOOST_LOG_SEV(globalLogger, info) << "Lightning positioning network client starting.";
	BOOST_LOG_SEV(globalLogger, info) << "GIT commit: " << GIT_COMMIT_TAG;

    // Register signals handlers
    signal(SIGINT, signalCallbackHandler);
    signal(SIGTERM, signalCallbackHandler);

    try
    {
        dfclient.reset(new DFClientSystem);
        dfclient->init("dfclient.conf");
    }
    catch(const std::exception& ex)
    {
    	BOOST_LOG_SEV_EXTRACE(globalLogger, fatal, ex, "Exception during system initialization: " << ex.what());
        return 1;
    }

    dfclient->join();

    BOOST_LOG_SEV(globalLogger, debug) << "dfclient main done";
    return 0;
}
