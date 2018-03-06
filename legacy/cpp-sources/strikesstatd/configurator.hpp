#ifndef CONFIGURATOR_HPP_INCLUDED
#define CONFIGURATOR_HPP_INCLUDED

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <stdexcept>

class StrikesStatsConfigurator
{
public:
    StrikesStatsConfigurator();
    static StrikesStatsConfigurator& instance();
    static void readConfig(const std::string& filename);
    static boost::property_tree::ptree& pt() { return  instance().m_pt; };
    
private:
    boost::property_tree::ptree m_pt;
};

#endif // CONFIGURATOR_HPP_INCLUDED
