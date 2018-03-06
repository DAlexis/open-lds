#include <boost/program_options.hpp>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description generalOptions("Genral options");
    generalOptions.add_options()
        ("help,h", "Print help message")
        //("verbose,v", po::value<int>()->default_value(0), "verboose level: 0 - info, 1 - debug, 2 - trace")
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
    
    cout << "Hello, strikes!" << endl;
    return 0;
}
