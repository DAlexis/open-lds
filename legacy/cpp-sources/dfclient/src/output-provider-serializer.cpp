/*
 * output-provider-serializer.cpp
 *
 *  Created on: 27 мая 2016 г.
 *      Author: dalexies
 */

#include "output-provider-serializer.hpp"
#include "logging.hpp"
#include "timestamp-utils.hpp"
#include "configurator.hpp"
#include "system-utils.hpp"

#include <typeinfo>

const std::string SerializerOutputProvider::m_name = "serializer";

CONCRETE_FACTORY_REGISTRATION(SerializerOutputProviderFactory, OutputProviderCreator, SerializerOutputProvider::m_name)
CONCRETE_FACTORY_CREATE_METHOD(SerializerOutputProviderFactory, SerializerOutputProvider, IOutputProvider)


SerializerOutputProvider::SerializerOutputProvider()
{
	BOOST_LOG_FUNCTION();

}

const std::string& SerializerOutputProvider::name()
{
	BOOST_LOG_FUNCTION();
	return m_name;
}


void SerializerOutputProvider::init(const std::string& configSubtree)
{
	BOOST_LOG_FUNCTION();
	m_filenameTemplate = SystemUtils::replaceTilta(Configurator::instance().get<std::string>(configSubtree + ".filename"));
	m_fileRotationSize = Configurator::instance().get<size_t>(configSubtree + ".rotation_size", m_fileRotationSize);
	renewFilename();
}

void SerializerOutputProvider::beginGroupOutput()
{
	BOOST_LOG_FUNCTION();
	try {
		m_file.open(m_filename, std::ofstream::out | std::ofstream::app | std::ofstream::binary);
		if (!m_file.is_open())
			throw ATT_SCOPES_STACK(ExCannotSetupOutput("Cannot open file: is_open() == false"));
	}
	catch (std::exception& ex)
	{
		throw ATT_SCOPES_STACK(ExCannotSetupOutput("Cannot open file " + m_filename + ": " + ex.what()));
	}
	catch (...)
	{
		throw ATT_SCOPES_STACK(ExCannotSetupOutput("Cannot open file " + m_filename + ": unknown exception."));
	}
}

bool SerializerOutputProvider::readyToOutput()
{
	return m_file.is_open();
}

void SerializerOutputProvider::endGroupOutput()
{
	BOOST_LOG_FUNCTION();
	try
	{
		if (m_file.is_open())
			m_file.close();
	}
	catch (std::exception& ex)
	{
		throw ATT_SCOPES_STACK(ExCannotSetupOutput("Cannot close file " + m_filename + ": " + ex.what()));
	}
	catch (...)
	{
		throw ATT_SCOPES_STACK(ExCannotSetupOutput("Cannot close file " + m_filename + ": unknown exception."));
	}

	testFileSize();
}

void SerializerOutputProvider::doOutput(const IStrikeDataContainer& container)
{
	BOOST_LOG_FUNCTION();

	if (!readyToOutput())
		throw ATT_SCOPES_STACK(ExCannotDoOutput("File not ready to output"));

	lightser::ByteStreamWrapper bsw;
	try
	{
		IStrikeDataContainer& nc = const_cast<IStrikeDataContainer&>(container);
		lightser::IBSWFriendly& c = dynamic_cast<lightser::IBSWFriendly&>(nc);
		c.serDeser(bsw);
	}
	catch(std::bad_cast& ex)
	{
		throw ATT_SCOPES_STACK(ExCannotDoOutput(std::string("Cannot serialize data container: ") + ex.what()));
	}

	try
	{
		m_file.write(static_cast<const char*>(bsw.buffer()), bsw.size());
		m_currentFileSize += bsw.size();
	}
	catch(std::exception& ex)
	{
		throw ATT_SCOPES_STACK(ExCannotDoOutput("Cannot write data to file " + m_filename + " : " + ex.what()));
	}
}

void SerializerOutputProvider::testFileSize()
{
	if (m_currentFileSize >= m_fileRotationSize)
	{
		renewFilename();
		m_currentFileSize = 0;
	}
}

void SerializerOutputProvider::renewFilename()
{
	boost::posix_time::ptime nowTime = boost::posix_time::second_clock::local_time();

	const std::string tag = "%t";
	m_filename = m_filenameTemplate;
	size_t pos = m_filename.find(tag);

	if (pos != m_filename.npos)
		m_filename.replace(pos, tag.length(), formatTimeFS(nowTime));
}
