#include "configurator.hpp"

#include <boost/property_tree/ini_parser.hpp>

StrikesStatsConfigurator::StrikesStatsConfigurator()
{
}

StrikesStatsConfigurator& StrikesStatsConfigurator::instance()
{
    static StrikesStatsConfigurator configurator;
    return configurator;
}

void StrikesStatsConfigurator::readConfig(const std::string& filename)
{
    try {
        boost::property_tree::ini_parser::read_ini(filename, instance().m_pt);
    }
     
    catch(boost::property_tree::ini_parser::ini_parser_error &exception)
    {
        //std::ostringstream message
        throw std::logic_error(".ini file parsing error in " + exception.filename() +  ":" + std::to_string(exception.line()) + " - " + exception.message() );
    }
}
