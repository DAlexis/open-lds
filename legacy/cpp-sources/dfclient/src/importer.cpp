#include "importer.hpp"
#include "strike-data-container-boltek.hpp"
#include "configurator.hpp"

#include <fstream>

void Importer::run(const std::string& configFileName, const std::string& filename)
{
    BOOST_LOG_FUNCTION();
    BOOST_LOG_SEV(globalLogger, info) << "Binary stream importer initialization";
    const std::string configSubtree = "Output-provider-mysql";
    try
    {
        BOOST_LOG_NAMED_SCOPE("Reading config");
        Configurator::instance().readConfig(configFileName);

        m_connection.hostName         = Configurator::instance().get<std::string>(configSubtree + ".host");
        m_connection.userName         = Configurator::instance().get<std::string>(configSubtree + ".user");
        m_connection.password         = Configurator::instance().get<std::string>(configSubtree + ".password");
        m_connection.name             = Configurator::instance().get<std::string>(configSubtree + ".name");
    }
    catch(const Configurator::ExConfiguratorFailed &ex)
    {
        BOOST_LOG_SEV_EXTRACE(globalLogger, fatal, ex, "Config reading error: " << ex.what());
        throw ATT_SCOPES_STACK(ExInitFailed("System initialization failed"));
    }
    catch(const std::exception &ex)
    {
        BOOST_LOG_SEV_EXTRACE(globalLogger, fatal, ex, "System initialization failed: " << ex.what());
        throw ATT_SCOPES_STACK(ExInitFailed("System initialization failed"));
    }
    catch(...)
    {
        BOOST_LOG_SEV(globalLogger, fatal) << "System initialization failed: unknown exception";
        throw ATT_SCOPES_STACK(ExInitFailed("System initialization failed"));
    }

    BOOST_LOG_SEV(globalLogger, info) << "Connecting to database...";
    m_connection.openConnection();

    BOOST_LOG_SEV(globalLogger, info) << "Reading binary " << filename << "...";
    std::ifstream inputFile(filename, std::ios::in | std::ios::binary);
    lightser::ByteStreamWrapper bsw(inputFile);
    inputFile.close();


    BOOST_LOG_SEV(globalLogger, info) << "Importing...";
    StrikeDataContainerBoltek sdc;
    while (!bsw.empty())
    {
        try {
            sdc.serDeser(bsw);
        } catch (const lightser::ByteStreamWrapper::out_of_range& ex)
        {
            BOOST_LOG_SEV(globalLogger, error) << "Unexpected end of data stream!";
            break;
        }

        m_connection << sdc.getMySQLQuery();
        std::cout << sdc.str() << std::endl;
    }
    BOOST_LOG_SEV(globalLogger, info) << "done.";
}
