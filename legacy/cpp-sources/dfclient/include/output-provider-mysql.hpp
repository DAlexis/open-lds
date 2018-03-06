/*
 * mysql-output-provider.hpp
 *
 *  Created on: 31 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_INCLUDE_OUTPUT_PROVIDER_MYSQL_HPP_
#define DFCLIENT_INCLUDE_OUTPUT_PROVIDER_MYSQL_HPP_

#include "output-provider-interface.hpp"
#include "MysqlWrapper.hpp"

class MySQLOutputProvider : public IOutputProvider
{
public:
	MySQLOutputProvider();

	const std::string& name() override;
	void init(const std::string& configSubtree) override;
	void beginGroupOutput() override;
	void endGroupOutput() override;
	bool readyToOutput() override;
	void doOutput(const IStrikeDataContainer& container) override;

private:
	MySQLConnectionManager m_connection;
	bool m_readyToOutput = false;
	const std::string m_name = "MySQL";
};

CONCRETE_FACTORY_HEADER(MySQLOutputProviderFactory, IOutputProvider, IOutputProviderFactory)


#endif /* DFCLIENT_INCLUDE_OUTPUT_PROVIDER_MYSQL_HPP_ */
