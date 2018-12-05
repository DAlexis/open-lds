#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include "MysqlWrapper.hpp"
#include "logging.hpp"
#include "macros.hpp"
#include <string>

class Importer
{
public:
    void run(const std::string& configFileName, const std::string& filename);

    WRAP_STDEXCEPT(ExInitFailed, std::runtime_error)
    WRAP_STDEXCEPT(ExRuntimeError, std::runtime_error)
private:
    MySQLConnectionManager m_connection;
    std::string m_filename;
};

#endif // IMPORTER_HPP
