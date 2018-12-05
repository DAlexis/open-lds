/*
 * output-provider-mysql.cpp
 *
 *  Created on: 31 марта 2016 г.
 *      Author: dalexies
 */

#include "output-provider-mysql.hpp"
#include "configurator.hpp"

CONCRETE_FACTORY_REGISTRATION(MySQLOutputProviderFactory, OutputProviderCreator, "mysql")
CONCRETE_FACTORY_CREATE_METHOD(MySQLOutputProviderFactory, MySQLOutputProvider, IOutputProvider)


MySQLOutputProvider::MySQLOutputProvider()
{

}

const std::string& MySQLOutputProvider::name()
{
	return m_name;
}


void MySQLOutputProvider::init(const std::string& configSubtree)
{
	m_connection.hostName         = Configurator::instance().get<std::string>(configSubtree + ".host");
	m_connection.userName         = Configurator::instance().get<std::string>(configSubtree + ".user");
	m_connection.password         = Configurator::instance().get<std::string>(configSubtree + ".password");
	m_connection.name             = Configurator::instance().get<std::string>(configSubtree + ".name");
}

void MySQLOutputProvider::beginGroupOutput()
{
	m_readyToOutput = false;

	m_connection.closeConnection();
	m_connection.openConnection();

	m_readyToOutput = true;
}

bool MySQLOutputProvider::readyToOutput()
{
	return m_readyToOutput;
}

void MySQLOutputProvider::endGroupOutput()
{
	m_readyToOutput = false;
	try {
		m_connection.closeConnection();
	}
	catch(MySQLConnectionManager::Exception& ex)
	{
		throw ExCannotSetupOutput(std::string("Cannot finalize output to MySQL: ") + ex.what());
	}
	catch(...)
	{
		throw ExCannotSetupOutput(std::string("Cannot finalize output to MySQL: unknown exception when connection closing"));
	}
}

void MySQLOutputProvider::doOutput(const IStrikeDataContainer& container)
{
	m_connection << container.getMySQLQuery();
}
