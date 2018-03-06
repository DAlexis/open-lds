/*
 * output-provider-serializer.hpp
 *
 *  Created on: 27 мая 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_INCLUDE_OUTPUT_PROVIDER_SERIALIZER_HPP_
#define DFCLIENT_INCLUDE_OUTPUT_PROVIDER_SERIALIZER_HPP_

#include "output-provider-interface.hpp"
#include <fstream>

class SerializerOutputProvider : public IOutputProvider
{
public:
	SerializerOutputProvider();

	const std::string& name() override;
	void init(const std::string& configSubtree) override;
	void beginGroupOutput() override;
	void endGroupOutput() override;
	bool readyToOutput() override;
	void doOutput(const IStrikeDataContainer& container) override;

	static const std::string m_name;
private:
	void testFileSize();
	void renewFilename();

	std::string m_filenameTemplate;
	std::string m_filename;
	size_t m_currentFileSize = 0;
	size_t m_fileRotationSize = 100*1024*1024;
	std::ofstream m_file;
};

CONCRETE_FACTORY_HEADER(SerializerOutputProviderFactory, IOutputProvider, IOutputProviderFactory)


#endif /* DFCLIENT_INCLUDE_OUTPUT_PROVIDER_SERIALIZER_HPP_ */
