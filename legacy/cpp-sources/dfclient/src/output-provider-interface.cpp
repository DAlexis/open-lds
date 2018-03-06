/*
 * output-provider-interface.cpp
 *
 *  Created on: 31 марта 2016 г.
 *      Author: dalexies
 */

#include "output-provider-interface.hpp"

SINGLETON_IN_CPP(OutputProviderCreator)

OutputProviderCreator::OutputProviderCreator()
{
}

void OutputProviderCreator::registerFactory(const std::string& name, IOutputProviderFactory* factory)
{
	auto it = m_factories.find(name);
	if (it != m_factories.end())
		throw std::runtime_error(std::string("Hardware interface factory ") + name + " already registered");

	m_factories[name] = factory;
}

IOutputProvider* OutputProviderCreator::create(const std::string& name)
{
	auto it = m_factories.find(name);
	if (it == m_factories.end())
		throw std::runtime_error(std::string("Hardware interface factory ") + name + " not registered");

	return it->second->create();
}
