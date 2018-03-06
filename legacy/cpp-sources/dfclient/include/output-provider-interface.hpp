/*
 * output-provider-interface.hpp
 *
 *  Created on: 31 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_INCLUDE_OUTPUT_PROVIDER_INTERFACE_HPP_
#define DFCLIENT_INCLUDE_OUTPUT_PROVIDER_INTERFACE_HPP_

#include "strike-data-container-interface.hpp"
#include "macros.hpp"
#include <map>
#include <string>

class IOutputProvider
{
public:
	virtual ~IOutputProvider() {}

	virtual const std::string& name() = 0;
	virtual void init(const std::string& configSubtree) = 0;
	virtual void beginGroupOutput() = 0;
	virtual void endGroupOutput() = 0;
	virtual bool readyToOutput() = 0;
	virtual void doOutput(const IStrikeDataContainer& container) = 0;

	WRAP_STDEXCEPT(ExCannotSetupOutput, std::runtime_error)
	WRAP_STDEXCEPT(ExCannotDoOutput, std::runtime_error)
};

ABSTRACT_FACTORY(IOutputProviderFactory, IOutputProvider)

class OutputProviderCreator
{
public:
	void registerFactory(const std::string& name, IOutputProviderFactory* factory);
	IOutputProvider* create(const std::string& name);

	SIGLETON_IN_CLASS(OutputProviderCreator)

private:
	OutputProviderCreator();

	std::map<std::string, IOutputProviderFactory*> m_factories;
};


#endif /* DFCLIENT_INCLUDE_OUTPUT_PROVIDER_INTERFACE_HPP_ */
