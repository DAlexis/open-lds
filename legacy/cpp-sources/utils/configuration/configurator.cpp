/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "configurator.hpp"
#include "system-utils.hpp"
#include "logging.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

SINGLETON_IN_CPP(Configurator);

Configurator::Configurator()
{
}

void Configurator::readConfig(const std::string& fileName)
{
	std::vector<std::string> paths = {
		std::string(""),
		SystemUtils::homeDir() + "/.config/dfclient/",
		std::string("/etc/"),
	};

	std::string fullName = SystemUtils::probeFiles(paths, fileName);

	if (fullName == "")
		throw ATT_SCOPES_STACK(ExConfiguratorFailed(std::string("Config file ") + fileName + " not found"));

	BOOST_LOG_SEV(globalLogger, info) << "Loading configuration file " << fullName << "...";

    try {
        boost::property_tree::ini_parser::read_ini(fullName, m_pt);
    }
    catch(boost::property_tree::ini_parser::ini_parser_error &exception)
    {
        throw ExConfiguratorFailed(std::string("Parsing error in ") + exception.filename()
                + ":" + std::to_string(exception.line()) + " - " + exception.message());
    }
    catch(boost::property_tree::ptree_error &exception)
    {
        throw ATT_SCOPES_STACK(ExConfiguratorFailed(std::string("Parsing error in ") + exception.what()));
    }
    catch(...)
    {
        throw ATT_SCOPES_STACK(ExConfiguratorFailed("Unknown parsing error"));
    }
}
