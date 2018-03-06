#include "stats-calculator.hpp"
#include "logging.hpp"
#include "configurator.hpp"

#include <boost/program_options.hpp>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description generalOptions("Genral options");
    generalOptions.add_options()
        ("help,h", "Print help message")
        ("verbose,v", po::value<int>()->default_value(0), "verboose level: 0 - info, 1 - debug, 2 - trace")
        ;
    
    po::options_description allOptions("Allowed options");
    allOptions.add(generalOptions);
    po::variables_map vmOptions;
    
    try
    {
        po::store(po::parse_command_line(argc, argv, allOptions), vmOptions);
        po::notify(vmOptions);
        switch(vmOptions["verbose"].as<int>())
		{
		default:
		case 0:
			initLoging("strikesstatd", info);
			break;
		case 1:
			initLoging("strikesstatd", debug);
			break;
		case 2:
			initLoging("strikesstatd", trace);
			break;
		}
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
    
    globalLogger.add_attribute("Job", boost::log::attributes::constant< std::string >("init"));
    
    BOOST_LOG_SEV(globalLogger, info) << "Starting strikes statistics collection deamon";
    
    try
    {
        BOOST_LOG_SEV(globalLogger, info) << "Reading configuration...";
        StrikesStatsConfigurator::readConfig("strikesstatd.conf");
    }
    catch(std::exception &ex)
    {
        BOOST_LOG_SEV(globalLogger, fatal) << "Error during reading configuration: " << ex.what();
        return -1;
    }
    
    try
    {
        std::unique_ptr<StatsCalculator> statsCalculator(new StatsCalculator);
        statsCalculator->registerSignals();
        statsCalculator->run();
    }
    catch(std::exception &ex)
    {
        BOOST_LOG_SEV(globalLogger, fatal) << "Error while running stats calculator: " << ex.what();
        return -1;
    }
    return 0;
}
