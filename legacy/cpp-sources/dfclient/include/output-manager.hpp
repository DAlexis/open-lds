#ifndef OUTPUT_MANAGER_H_INCLUDED
#define OUTPUT_MANAGER_H_INCLUDED

#include "strike-data-container-interface.hpp"
#include "output-provider-interface.hpp"
#include "MysqlWrapper.hpp"
#include "configurator.hpp"
#include "utils.hpp"
#include <mutex>
#include <time.h>

/// This class manages output to MySQL database or to file for it's own considering
class OutputManager : public LoopFuncHolder
{
public:
    OutputManager();
    ~OutputManager();
    
    void proceedOuput(const IStrikeDataContainer& data);
    
    void init();
    
    class ExOutputNotCompleted : public std::exception
    {
    public:
        virtual const char* what() const throw() {return "Ouput not completed"; }
    };
    
    class ExCannotWriteToFile : public ExOutputNotCompleted
    {
    public:
        virtual const char* what() const throw() { return "Cannot write strikes to output file."; }
    };
    
private:
    /// Single ouput to file
    void outputToFile(const IStrikeDataContainer& data);
    /// Try to restore data from file. Those that cannot be restored will be written again
    void restoreDataFromFile();
    /// Clear buffer file after fully reloading to MySQL
    void clearFile();
    
    /// This function tries to reconnect and restore data from file to MySQL
    virtual void loopFunction();
    
    MySQLConnectionManager MySQL;
    
    enum OutputMode {
        TO_MYSQL = 0,
        TO_FILE,
    };
    
    OutputMode outputMode;
    
    int MySQLFailsConnect;
    int MySQLFailsConnectLimit;
    std::string fileName;
    double reconnectPeriod;
    
    time_t lastMySQLAttempt;
    std::mutex m_fileMutex;
    
    class ExDataRestoringMySQLFail : public std::exception
    {
    public:
        virtual const char* what() const throw() { return "Cannot (fully) restore data from file"; }
    };
};

#endif //OUTPUT_MANAGER_H_INCLUDED
