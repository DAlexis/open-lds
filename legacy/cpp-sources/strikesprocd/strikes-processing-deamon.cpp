/*
 * Copyright (c) 2014 Institute of Applied Physics of the Russian Academy of Sciences
 */
 
#include "strikes-processor.hpp"
#include "logging.hpp"

#include "strikes-group-solver-variational.hpp"
#include "constants.hpp"

#include <iostream>
#include <signal.h>

#include <boost/program_options.hpp>

using namespace std;

namespace attrs = boost::log::attributes;

StrikesProcessor *strikesProcessor = nullptr;

void deInitAll()
{
    if (strikesProcessor) delete strikesProcessor;
}

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description generalOptions("Genral options");
    generalOptions.add_options()
        ("help,h", "Print help message")
        ("disable-copying", "Disable reading strikes from raw db, clasterisation and copying to unified table")
        ("disable-solving", "Disable solving lightning detection problem")
        ("verbose,v", po::value<int>()->default_value(0), "verboose level: 0 - info, 1 - debug, 2 - trace")
        ;
    
    po::options_description allOptions("Allowed options");
    allOptions.add(generalOptions);
    
    po::variables_map vmOptions;
    try
    {
        po::store(po::parse_command_line(argc, argv, allOptions), vmOptions);
        po::notify(vmOptions);
    }
    catch (po::error& e)
    {
        cerr << "Command line parsing error: " << e.what() << endl;
        return -1;
    }
    
    if (vmOptions.count("help"))
    {
        cout << allOptions << endl;
        return 0;
    }
    
    bool needClasterisation = true, needSolving = true;
    if (vmOptions.count("disable-copying"))
        needClasterisation = false;
    
    if (vmOptions.count("disable-solving"))
        needSolving = false;
    
    switch(vmOptions["verbose"].as<int>())
    {
    default:
    case 0:
    	initLoging("strikesprocd", info);
    	break;
    case 1:
    	initLoging("strikesprocd", debug);
    	break;
    case 2:
    	initLoging("strikesprocd", trace);
    	break;
    }

    
    globalLogger.add_attribute("Job", attrs::constant< std::string >("initialisation"));
    
    BOOST_LOG_SEV(globalLogger, info) << "Starting strikes processing deamon";
    strikesProcessor = new StrikesProcessor;
    BOOST_LOG_SEV(globalLogger, info) << "Reading configuration...";
    
    try
    {
        strikesProcessor->readConfig("strike-proc-d.conf");
        Configuration::readConfigFromFile("df-math.conf");
    }
    catch (std::exception& ex)
    {
        BOOST_LOG_SEV(globalLogger, fatal) << "Cannot read configuration: " << ex.what();
        deInitAll();
        return 0;
    }
    
    strikesProcessor->registerSignals();
    BOOST_LOG_SEV(globalLogger, debug) << "Running...";
    strikesProcessor->run(needClasterisation, needSolving);
    
    //deInitAll();
    return 0;
}
