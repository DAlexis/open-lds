/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#include "MysqlWrapper.hpp"
#include "logging.hpp"

#include <iostream>

std::mutex MySQLConnectionManager::initMutex;

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}


///////////////////////
// MySQLInsertGenerator
MySQLInsertGenerator::MySQLInsertGenerator(std::string tableName) :
    m_tableName(tableName)
{
}

void MySQLInsertGenerator::set(const std::string& key, const std::string& value, bool addQuotes)
{
    if (addQuotes)
    {
        m_fields[key] = std::string("'") + value + "'";
    } else {
        m_fields[key] = value;
    }
}

void MySQLInsertGenerator::add(const std::map<std::string, std::string>& fields)
{
    m_fields.insert(fields.begin(), fields.end());
}

std::string MySQLInsertGenerator::generate() const
{
    std::string keys, values;
    auto it = m_fields.begin();
    if (it != m_fields.end())
    {
        keys = it->first;
        values = it->second;
        it++;
    }
    
    for (; it != m_fields.end(); it++)
    {
        keys += ", ";
        keys += it->first;
        values += ", ";
        values += it->second;
    }
    
    std::string result = std::string("INSERT INTO ") + m_tableName + "(" + keys + ") VALUES(" + values + ")";
    return result;
}



///////////////////////
// MySQLConnectionManager

MySQLConnectionManager::MySQLConnectionManager()
{
}

MySQLConnectionManager::MySQLConnectionManager(const MySQLConnectionManager& source) :
    hostName(source.hostName),
    userName(source.userName),
    password(source.password),
    name(source.name),
    connection(nullptr),
    m_pResult(nullptr)
{
}

MySQLConnectionManager::~MySQLConnectionManager()
{
    closeConnection();
}

void MySQLConnectionManager::openConnection()
{
    std::unique_lock<std::mutex> lock (initMutex);
    if (connection) {
        throw ATT_SCOPES_STACK(ExConnectionAlreadyEstablished());
    }
    
    connection = mysql_init(nullptr);

    if (connection == nullptr)
    {
        throw ATT_SCOPES_STACK(ExCannotInitMysql("mysql_init returned NULL"));
    }

#ifdef DEBUG_MYSQL
    std::cout << "Connecting " << hostName << " " << userName << " " << password <<std::endl;
#endif

    if (mysql_real_connect(connection, hostName.c_str(), userName.c_str(), password.c_str(),
      name.c_str(), 0, nullptr, 0) == nullptr)
    {
        throw ATT_SCOPES_STACK(ExCannotOpenConnection("mysql_real_connect returned NULL"));
    }
}

void MySQLConnectionManager::closeConnection()
{
    if (m_pResult)
    {
        mysql_free_result(m_pResult);
        m_pResult = nullptr;
    }
    if (connection)
    {
        mysql_close(connection);
        connection = nullptr;
    }
}

void MySQLConnectionManager::reconnect()
{
    closeConnection();
    openConnection();
}

bool MySQLConnectionManager::getQueryResult()
{
    //std::cout << "Getting query result" << std::endl;
    if (m_pResult)
        mysql_free_result(m_pResult);
    //std::cout << "mysql_store_result(connection);" << std::endl;
    m_pResult = mysql_store_result(connection);
    //std::cout << "mysql_num_fields" << std::endl;
    if (m_pResult)
        m_fieldsCount = mysql_num_fields(m_pResult);
    return nextRow();
}

bool MySQLConnectionManager::query(const char *q)
{
#ifdef DEBUG_MYSQL
    std::cout << "== Mysql query > "; 
    std::cout << q << std::endl;
    std::cout << "< Mysql query ==" << std::endl;
#endif
    int rc = mysql_real_query(connection, q, strlen(q));

#ifdef DEBUG_MYSQL
    std::cout << "== Mysql query processed ==" << std::endl;
#endif


    if (rc != 0)
        throw ATT_SCOPES_STACK(ExCannotCompleteQuery(mysql_error(connection)));
    
    return getQueryResult();
}

bool MySQLConnectionManager::query(const std::string &&q)
{    
    return query(q.c_str());
}

bool MySQLConnectionManager::query(const MySQLInsertGenerator& insert)
{
    return query(insert.generate());
}

bool MySQLConnectionManager::nextRow()
{
    if (not m_pResult)
        return false;
    
   //  std::cout << "mysql_fetch_row(m_pResult);" << std::endl;
    m_currentRow = mysql_fetch_row(m_pResult);
    
    if (not m_currentRow)
        return false;
    //std::cout << "mysql_fetch_lengths(m_pResult);" << std::endl;
    m_currentLengths = mysql_fetch_lengths(m_pResult);
    return true;
}

void MySQLConnectionManager::throwGetException(unsigned int column)
{
    if (not m_pResult)
        throw ATT_SCOPES_STACK(ExNoDataReturned("No results ready for reading"));
    
    if (not m_currentRow || column >= m_fieldsCount)
        throw ATT_SCOPES_STACK(ExCannotGetData("Current row or column is not avaliable"));
}

char* MySQLConnectionManager::getData(unsigned int column)
{
    throwGetException(column);
    
    return m_currentRow[column];
}

unsigned int MySQLConnectionManager::getLength(unsigned int column)
{
    throwGetException(column);
    
    return m_currentLengths[column];
}

unsigned int MySQLConnectionManager::getFieldsCount()
{
    return m_fieldsCount;
}

size_t MySQLConnectionManager::last_insert_id()
{
	return mysql_insert_id(connection);
}
