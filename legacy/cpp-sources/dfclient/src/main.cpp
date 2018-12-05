/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "dfclient-system.hpp"
#include "logging.hpp"
#include "importer.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <signal.h>
#include <memory>
#include <chrono>
#include <string>

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

int main(int argc, char** argv)
{
    // Command line parsing
    namespace po = boost::program_options;
    po::options_description generalOptions("Genral options");
    generalOptions.add_options()
        ("help,h", "Print help message")
        ("import-binary,i", po::value<std::string>(), "Load data from binary file to database. Important: this will not run main dfclient mode!");

    po::variables_map vmOptions;
    try
    {
        po::store(po::parse_command_line(argc, argv, generalOptions), vmOptions);
        po::notify(vmOptions);
    }
    catch (po::error& e)
    {
        BOOST_LOG_SEV(globalLogger, fatal) << "Command line parsing error: " << e.what();
        return -1;
    }
    if (vmOptions.count("help"))
    {
        std::cout << generalOptions << std::endl;
        return 0;
    }

    if (vmOptions.count("import-binary"))
    {
        Importer importer;
        importer.run("dfclient.conf", vmOptions["import-binary"].as<std::string>());
        return 0;
    }

    // Running main dfclient mode
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
