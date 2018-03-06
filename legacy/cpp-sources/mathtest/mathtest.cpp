/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "dfmath.hpp"
#include "MysqlWrapper.hpp"
#include "math-utils.hpp"
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

using namespace std;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description generalOptions("Genral options");
    generalOptions.add_options()
        ("help,h", "Print help message")
        ("real-randomization", "Use random row or fixed row as random numbers")
        ("randomization", po::value<unsigned int>()->default_value(1), "Randomization parameter. The same value cause the same random row")
        ("distance", po::value<double>()->default_value(500000), "Maximal distance between DF stations for smart strikes reading, m")
        ("verbose,v", "Print additional info during work")
        ("config", po::value<std::string>(), "Load custom config from configuration file")
        ;
    
    po::options_description inputOptions("Input options");
    inputOptions.add_options()
        ("first,f", po::value<unsigned int>(), "Id of first strike operand")
        ("second,s", po::value<unsigned int>(), "Id of second strike operand (not used if 1-strike operation)")
        ("time-from", po::value<std::string>(), "Selection begin")
        ("time-to", po::value<std::string>(), "Selection end")
        ("source-table", po::value<std::string>()->default_value("unified"), "Source table for single strike reading. 'boltek' or 'unified'")
        ("read-strikes-group", "Smart reading strikes group from db in interval from time-from to time-to")
        ("read-cluster", po::value<unsigned int>(), "Read the whole cluster with specified id")
        ("custom-query", po::value<string>(), "Make a query with custom condition")
        ;
        
    po::options_description outputOptions("Output options");
    outputOptions.add_options()
        ("write-group-to-server-base", "Write result to servers database")
        ("write-skrike-to-server-base", "Write result to servers database")
        ("write-time-clusters", "Write detected time clusters to table")
        ("write-solution-to-base", "Write position of lightning to base")
        ("output-file-prefix", po::value<std::string>()->default_value("output"),
            "Prefix for files with fields where those will be outputted")
        ("copy-ids-range-to-server", "Copy ids from id-from to id-to")
        
        ;
    
    po::options_description rangesOptions("Ranges options");
    rangesOptions.add_options()
        ("graph-lat-points", po::value<unsigned int>()->default_value(100), "Points to output in latitude if graph is needed")
        ("graph-lon-points", po::value<unsigned int>()->default_value(100), "Points to output in longitude if graph is needed")
        ("graph-margins", po::value<double>()->default_value(1), "Graph auto-scale margins. Note: auto-scale is disabled if options --graph-lat-* and --graph-lon-* specified")
        ("graph-lat-bottom", po::value<double>(), "Bottom border in latitude for plotting")
        ("graph-lat-top", po::value<double>(), "Top border in latitude for plotting")
        ("graph-lon-left", po::value<double>(), "Left border in longitude for plotting")
        ("graph-lon-right", po::value<double>(), "Right border in longitude for plotting")
        ("id-from", po::value<unsigned int>(), "First id to copy")
        ("id-to", po::value<unsigned int>(), "Last id to copy")
        ;
    
    po::options_description singleStrikeOperatingOptions("Single strikes operating options");
    singleStrikeOperatingOptions.add_options()
        ("direction", "Calculate direction angle and deriction dispersion")
        ("output-field,o", po::value<std::string>(), "What fiels to output (ex.: B,BE,E)")
        ("shift-time", po::value<double>(), "Add time shift to strike")
        ("set-latitude", po::value<double>(), "Set new latitude in degrees")
        ("set-longitude", po::value<double>(), "Set new longitude in degrees")
        ("move-latitude", po::value<double>(), "Add arg to latitude")
        ("move-longitude", po::value<double>(), "Add arg to longitude")
        ;
    
    po::options_description strikesPairOperatingOptions("Strikes pair operating options");
    strikesPairOperatingOptions.add_options()
        ("corellation", "Calculate corellation")
        ("strikes-distance", "Calculate distance and output coordinates")
        ;
    
    po::options_description strikesGroupsOperatingOptins("Strike groups operating options");
    strikesGroupsOperatingOptins.add_options()
        ("clasterisation", "Complete time clasterisation for selection defined by time-from and time-to")
        ("detection-function-graph,g", "Plot 3d graph of detection function to output file")
        ("solve", "Solve lightning positioning problem")
        ;
    
    po::options_description testDataOptions("Position determining testing options");
    testDataOptions.add_options()
        ("test,t", "Allow faked positions")
        ("test-gen-random", po::value<unsigned int>(), "Generate random test data")
        ("test-time-dispersion", po::value<double>()->default_value(1e-6), "Add this dispersion to emulated times")
        ("test-read", po::value<string>(), "File contains test data")
        ("test-save", po::value<string>(), "File contains test data to be created")
        ("test-regen-time-errors", po::value<double>(), "Regenerate time error using arg dispersion")
        ("random-lat-bottom", po::value<double>(), "Bottom border in latitude for test generation")
        ("random-lat-top", po::value<double>(), "Top border in latitude for test generation")
        ("random-lon-left", po::value<double>(), "Left border in longitude for test generation")
        ("random-lon-right", po::value<double>(), "Right border in longitude for test generation")
        ("random-time-dispersion", po::value<double>()->default_value(0), "Dispersion for receiving time error generation")
        ;
    
    po::options_description databaseOptions("Database options");
    databaseOptions.add_options()
        ("source-hostname", po::value<std::string>()->default_value("localhost"), "Hostname of source db")
        ("source-username", po::value<std::string>()->default_value("root"), "Username for source db")
        ("source-password", po::value<std::string>()->default_value(""), "Password for source db")
        ("source-database", po::value<std::string>()->default_value("lightning_detection_system"), "Source db name")
        
        ("destination-hostname", po::value<std::string>()->default_value("localhost"), "Hostname of destination db")
        ("destination-username", po::value<std::string>()->default_value("root"), "Username for destination db")
        ("destination-password", po::value<std::string>()->default_value(""), "Password for destination db")
        ("destination-database", po::value<std::string>()->default_value("lightning_detection_system"), "Destination db name")
        ;
    
    po::options_description allOptions("Allowed options");
    allOptions.add(generalOptions)
        .add(inputOptions)
        .add(outputOptions)
        .add(rangesOptions)
        .add(singleStrikeOperatingOptions)
        .add(strikesPairOperatingOptions)
        .add(strikesGroupsOperatingOptins)
        .add(testDataOptions)
        .add(databaseOptions);
    
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
    
    ////////////////////////////////////
    // Options checking
    if (vmOptions.count("help"))
    {
        cout << allOptions << endl;
        return 0;
    }
    
    // Cheching to be correct
    if (vmOptions.count("corellation") && (vmOptions.count("first") == 0 || vmOptions.count("second") == 0))
    {
        cerr << "What to corellate not specified" << endl;
        return -1;
    }
    
    if (vmOptions.count("output-field") && vmOptions.count("first") == 0)
    {
        cerr << "What to output not specified" << endl;
        return -1;
    }
    
    if (vmOptions.count("copy-ids-range-to-server") && (vmOptions.count("id-from") == 0 || vmOptions.count("id-to") == 0))
    {
        cerr << "Ids range not set (use --id-from and --id-to)" << endl;
        return -1;
    }
    
    if (vmOptions.count("clasterisation") && (vmOptions.count("time-from") == 0 || vmOptions.count("time-to")))
    {
        cerr << "Time limits are not specified" << endl;
    }
    
    ////////////////////////////////////
    // Real job beginning
    
    // Db settings
    MySQLConnectionManager mysqlSource;
    mysqlSource.hostName = vmOptions["source-hostname"].as<std::string>();
    mysqlSource.userName = vmOptions["source-username"].as<std::string>();
    mysqlSource.password = vmOptions["source-password"].as<std::string>();
    mysqlSource.name     = vmOptions["source-database"].as<std::string>();
    
    
    MySQLConnectionManager mysqlDestination;
    mysqlDestination.hostName = vmOptions["destination-hostname"].as<std::string>();
    mysqlDestination.userName = vmOptions["destination-username"].as<std::string>();
    mysqlDestination.password = vmOptions["destination-password"].as<std::string>();
    mysqlDestination.name     = vmOptions["destination-database"].as<std::string>();
    
    bool verbose = false;
    if (vmOptions.count("verbose"))
        verbose = true;
    
    try
    {
        ///////////////////////////
        // General settings
        if (vmOptions.count("config"))
        {
            Configuration::readConfigFromFile(vmOptions["config"].as<string>());
            if (verbose) cout << "Configuration loaded from " << vmOptions["config"].as<string>() << endl;
        }
        
        if (vmOptions.count("real-randomization"))
        {
            Random::randomizeUsingTime();
            if (verbose) cout << "Randomization from current time used" << endl;
        } else {
            Random::randomize(vmOptions["randomization"].as<unsigned int>());
            if (verbose) cout << "Randomization with seed " << vmOptions["randomization"].as<unsigned int>() << " used" << endl;
        }
        
        ///////////////////////////
        // Data input
        StrikeData firstOperand, secondOperand;
        StrikesGroup group;
        TestingInstance generator;
        
        if (vmOptions.count("copy-ids-range-to-server")) {
            mysqlSource.openConnection();
            mysqlDestination.openConnection();
            size_t from = vmOptions["id-from"].as<unsigned int>();
            size_t to = vmOptions["id-to"].as<unsigned int>();
            cout << "Copying ids from " << from << " to " << to << " to server" << endl;
            size_t count = group.readBoltekStrikesWithIdsInRange(mysqlSource, from, to);
            cout << "Readed " << count << "/" << to-from+1 <<  ". Writing to base... " << endl;
            group.writeAllStrikesToDatabase(mysqlDestination);
            cout << "done." << endl;
            mysqlDestination.closeConnection();
            mysqlSource.closeConnection();
            return 0;
        }
        
        TableType sourceTable;
        if (vmOptions["source-table"].as<std::string>() == "boltek")
            sourceTable = TT_BOLTEK_RAW;
        else if (vmOptions["source-table"].as<std::string>() == "unified")
            sourceTable = TT_UNIFIED;
        else {
            cerr << "Unknown source: " << vmOptions["source-table"].as<std::string>() << endl;
            return 1;
        }
        
        // Reading single strikes if needed
        mysqlSource.openConnection();
        if (vmOptions.count("first") != 0) {
            firstOperand.read(sourceTable, mysqlSource, vmOptions["first"].as<unsigned int>());
            if (verbose) cout << "First operand readed from boltek db" << endl;
        }
        if (vmOptions.count("second") != 0) {
            secondOperand.read(sourceTable, mysqlSource, vmOptions["second"].as<unsigned int>());
            if (verbose) cout << "Second operand readed from boltek db" << endl;
        }
        // Reading strikes group with custom query if needed
        if (vmOptions.count("custom-query") != 0) {
            if (verbose) cout << "Reading strike group with custom query..." << endl;
            size_t count = group.readWithCustomQuery(sourceTable, mysqlSource, vmOptions["custom-query"].as<string>());
            if (verbose) {
                cout << "Strikes group loaded with condition " << vmOptions["custom-query"].as<string>() << endl;
                cout << "Loaded " << count << " strikes." << endl;
            }
        }
        mysqlSource.closeConnection();
        
        if (vmOptions.count("read-cluster"))
        {
            if (verbose) cout << "Reading time cluster..." << endl;
            mysqlSource.openConnection();
            int count = group.readTimeCluster(mysqlSource, vmOptions["read-cluster"].as<unsigned int>());
            std::cout << "Records found: " << count << std::endl;
            mysqlSource.closeConnection();
        }
        
        // Smart reading strikes group
        if (vmOptions.count("read-strikes-group"))
        {
            mysqlSource.openConnection();
            if (verbose) cout << "Reading strikes group by ids range..." << endl;
            Time from(vmOptions["time-from"].as<std::string>());
            Time to(vmOptions["time-to"].as<std::string>());
            cout <<  "Reading  from " << from.getString() << " to " << to.getString() << "..." << endl;
            
            int count = group.smartReadStrikes(TT_UNIFIED, mysqlSource, from, to);
            std::cout << "Smart reading done, records found: " << count << std::endl;
            mysqlSource.closeConnection();
        }
        
        ///////////////////////////
        // Operations with TestingInstance
        if (vmOptions.count("test-gen-random"))
        {
            generator.testSet.addRandomDF(vmOptions["random-lat-bottom"].as<double>(), vmOptions["random-lat-top"].as<double>(),
                                  vmOptions["random-lon-left"].as<double>(), vmOptions["random-lon-right"].as<double>(),
                                  vmOptions["random-time-dispersion"].as<double>(),
                                  vmOptions["test-gen-random"].as<unsigned int>());
            
            generator.testSet.addRandomLightning(vmOptions["random-lat-bottom"].as<double>(), vmOptions["random-lat-top"].as<double>(),
                                         vmOptions["random-lon-left"].as<double>(), vmOptions["random-lon-right"].as<double>());
            
            cout << "Random strikes generated" << endl;
        }
        
        if (vmOptions.count("test-read"))
        {
            generator.testSet.readConfig(vmOptions["test-read"].as<string>());
            if (verbose) cout << "Test data readed from " << vmOptions["test-read"].as<string>() << endl;
        }
        
        if (vmOptions.count("test"))
        {
            generator.generateStrikesGroup();
            group = generator.testGroup;
            if (verbose) cout << "Entering test mode. All operations with strike group now will be done with test data" << endl;
        }
        
        if (vmOptions.count("test-regen-time-errors"))
        {
            generator.testSet.generateTimeError(vmOptions["test-regen-time-errors"].as<double>());
            cout << "Time errors in test regenerated" << endl;
        }
        
        ///////////////////////////
        // Operating with strikes group
        if (vmOptions.count("solve"))
        {
            group.detectTimeClusters();
            if (verbose) cout << "Lightning positioning problem solving..." << endl;
            std::shared_ptr<IStrikesGroupSolver> solver(StrikesGroupSolverBuilder::instance().buildSolver("variational-2d"));
            solver->solve(group);
            LightningPositioningResult& result = solver->getMostProbableSolution();
            cout << "Solving result: " << endl << "    " << result.getString() << endl;
            if (result.isOnADetectionBorder) {
                cout << " ! Lightning detected on a farest border of detection ! " << endl;
            }
            if (verbose) {
                cout << "Detected lightnings candidates: " << endl;
                int counter = 1;
                for (auto it = solver->solutions().begin(); it != solver->solutions().end(); it++)
                {
                    cout << counter++ << ".) " << it->position.getString() << endl;
                }
            }
            if (vmOptions.count("write-solution-to-base"))
            {
                mysqlDestination.openConnection();
                solver->getMostProbableSolution().writeSolution(mysqlDestination);
                mysqlDestination.closeConnection();
            }
            
            group.writeSolutionsToFile(vmOptions["output-file-prefix"].as<std::string>());
            cout << "Solution points are written to file" << endl;
        }
        
        ///////////////////////////
        // Operations with strikes
        // Time
        if (vmOptions.count("shift-time") != 0)
        {
            firstOperand.time += vmOptions["time-shift"].as<double>();
            if (verbose) cout << "Strike time was shifted as specified" << endl;
        }
        // Coordinates
        bool coordsWasSetted = false;
        if (vmOptions.count("set-latitude") != 0)
        {
            firstOperand.position.setLatitude(vmOptions["set-latitude"].as<double>());
            coordsWasSetted = true;
        }
        
        if (vmOptions.count("set-longitude") != 0)
        {
            firstOperand.position.setLongitude(vmOptions["set-longitude"].as<double>());
            coordsWasSetted = true;
        }
        if (coordsWasSetted)
            if (verbose) cout << "Strike coordinates was set as specified" << endl;
        
        bool coordsWasMoved = false;
        if (vmOptions.count("move-latitude") != 0)
        {
            firstOperand.position.move(vmOptions["move-latitude"].as<double>(), 0);
            coordsWasMoved = true;
        }
        
        if (vmOptions.count("move-longitude") != 0)
        {
            firstOperand.position.move(0, vmOptions["move-longitude"].as<double>());
            coordsWasMoved = true;
        }
        if (coordsWasMoved)
            if (verbose) cout << "Strike coordinates was moved as specified" << endl;
        
        if (vmOptions.count("strikes-distance"))
        {
            cout << "First strike parameters:" << endl;
            cout << "    Lat: " << firstOperand.position.getLatitude() << "\tLon: " << firstOperand.position.getLongitude() << endl;
            cout << "    Time: " << firstOperand.time.getString() << endl;
            
            cout << "Second strike parameters:" << endl;
            cout << "    Lat: " << secondOperand.position.getLatitude() << "\tLon: " << secondOperand.position.getLongitude() << endl;
            cout << "    Time: " << secondOperand.time.getString() << endl;
            
            cout << "  Distance: " << firstOperand.position - secondOperand.position << endl;
            cout << "Time shift: " << firstOperand.time - secondOperand.time << endl;
            
        }
        ///////////////////////////
        // Text output        
        if (vmOptions.count("test-save"))
        {
            generator.testSet.writeConfig(vmOptions["test-save"].as<std::string>());
        }
        if (vmOptions.count("output-field"))
        {
            firstOperand.printToFiles(vmOptions["output-file-prefix"].as<std::string>(),
                                vmOptions["output-field"].as<std::string>());
            cout << "Fields output done." << endl;
        }
        
        if (vmOptions.count("corellation"))
        {
            ofstream file(vmOptions["output-file-prefix"].as<std::string>() + "-correlation.txt");
            
            cout << "Time shift between strikes' buffers is " << getTimeShift(firstOperand, secondOperand) << endl;
            
            for (double shift = -firstOperand.getBufferDuration();
                 shift < firstOperand.getBufferDuration();
                 shift += firstOperand.getTimeStep())
            {
                Corellation corr(firstOperand, secondOperand, shift);
                if (corr.hasOverlapping)
                    file << shift << " " << corr.corellation << endl;
            }
            file.close();            
            cout << "Corellation output done." << endl;
        }
        
        if (vmOptions.count("direction"))
        {
            cout << "Direction finding results:" << endl;
            firstOperand.findDirection();
            const Direction& dir = firstOperand.getDirection();
            cout << "azimuth = " << dir.azimuth << "; so angle = " << dir.angle << "; error factor = " << dir.errorRate << endl;
        }
        /*
        if (vmOptions.count("detection-function-graph"))
        {
            if (vmOptions.count("graph-lat-bottom") && vmOptions.count("graph-lat-top")
                && vmOptions.count("graph-lon-left") && vmOptions.count("graph-lon-right"))
            {
                group.plotDetectionFunction(vmOptions["graph-lat-bottom"].as<double>(), vmOptions["graph-lat-top"].as<double>(),
                                             vmOptions["graph-lon-left"].as<double>(), vmOptions["graph-lon-right"].as<double>(),
                                             vmOptions["graph-lat-points"].as<unsigned int>(), vmOptions["graph-lon-points"].as<unsigned int>(),
                                             vmOptions["output-file-prefix"].as<std::string>());
            } else {
                group.plotDetectionFunction(vmOptions["graph-margins"].as<double>(),
                                            vmOptions["graph-lat-points"].as<unsigned int>(), vmOptions["graph-lon-points"].as<unsigned int>(),
                                            vmOptions["output-file-prefix"].as<std::string>());
            }
            cout << "Data for 3d plotting detection function graph printed" << endl;
        }*/
        
        ///////////////////////////
        // Operations with time clusters
        if (vmOptions.count("clasterisation"))
        {
            group.detectTimeClusters();
            cout << "Detected " << group.timeClusters.size() << " clusters" << endl;
            size_t realClusters=0;
            for (auto it = group.timeClusters.begin(); it != group.timeClusters.end(); it++)
            {
                if (it->indexes.size() > 2) {
                    realClusters++;
                }
            }
            cout << "Detected " << realClusters << " clusters with more then two strikes" << endl;
        }
        
        ///////////////////////////
        // Output
        if (vmOptions.count("write-time-clusters"))
        {
            mysqlDestination.openConnection();
            group.writeTimeClusters(mysqlDestination);
            cout << "Time clusters written to database" << endl;
            mysqlDestination.closeConnection();
        }
        
        if (vmOptions.count("write-skrike-to-server-base"))
        {
            mysqlDestination.openConnection();
            firstOperand.writeUnifiedData(mysqlDestination);
            cout << "Writing strikes to destination database." << endl;
            mysqlDestination.closeConnection();
        }
        if (vmOptions.count("write-group-to-server-base"))
        {
            mysqlDestination.openConnection();
            group.writeAllStrikesToDatabase(mysqlDestination);
            cout << "Writing strikes group to destination database." << endl;
            mysqlDestination.closeConnection();
        }
    }
    catch(const std::exception &ex)
    {
        cerr << "Exception handled: " << ex.what() << endl;
    }
    catch(...)
    {
        cerr << "Not std exception handled." << endl;
    }
    return 0;
}
