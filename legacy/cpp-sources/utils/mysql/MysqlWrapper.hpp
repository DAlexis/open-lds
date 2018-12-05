/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

/**
 * Example of usage
 *
 *   BlobData<char> bd(data, size);
 *
 *   Mysql::getInstance()->query(std::string("INSERT INTO strikes(E_field) VALUES('") +
 *           std::string(bd.getData()) + std::string("')"));
 */

#ifndef MYSQL_WRAPPER_H_INCLUDED
#define MYSQL_WRAPPER_H_INCLUDED

//#define DEBUG_MYSQL

#include <stdexcept>

#include <string>
#include <map>

#include <string.h>
#include <vector>
#include <mutex>

// Another workaround for this ancient my_global.h
#define __builtin_expect(x, y)    (x)
#include <my_global.h>
#include <mysql.h>
#undef __builtin_expect

// Fix of http://bugs.mysql.com/bug.php?id=28184
// Mysql defines its own macroses min and max that conflicts with STL algorithm
// Fix of http://bugs.mysql.com/bug.php?id=28184
#undef likely
#undef min
#undef max
#undef test
#undef set_bits

#include <sstream>
#include <limits>
#include <iomanip>

class MySQLInsertGenerator
{
public:
    MySQLInsertGenerator(std::string tableName);
    
    void set(const std::string& key, const std::string& value, bool addQuotes = false);
    
    template<typename T>
    void set(const std::string& key, const T& value, bool addQuotes = false)
    {
        std::ostringstream oss;
        oss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << value;
        set(key, oss.str(), addQuotes);
    }
    
    
    void add(const std::map<std::string, std::string>& fields);
    
    std::string generate() const;
    
private:
    std::string m_tableName;
    std::map<std::string, std::string> m_fields;
};

class MySQLConnectionManager
{
public:
    std::string hostName;
    std::string userName;
    std::string password;
    std::string name;
    
    void openConnection();
    void closeConnection();
    void reconnect();
    
    /// Proceed MySQL query. false if no data returned
    bool query(const char *q);
    
    bool query(const std::string &&q);
    bool query(const std::string &q) { return query(std::forward<const std::string>(q)); }
    

    bool query(const MySQLInsertGenerator& insert);
    
    
    /// Go to next row returned by query; false if there are no more rows
    bool nextRow();
    
    /// Get count of fields in query's return
    unsigned int getFieldsCount();
    
    /// Get data at column at current row
    char* getData(unsigned int column);
    
    /// Get length of data at column at current row
    unsigned int getLength(unsigned int column);
    
    MySQLConnectionManager();
    
    /// Copying constructor will copy only config
    MySQLConnectionManager(const MySQLConnectionManager& source);
    ~MySQLConnectionManager();

    // Ideal forwarding wrapper
    template<class T>
    MySQLConnectionManager& operator<<(T&& q)
    {
        query(std::forward<T>(q));
        return *this;
    }



    /// Returns result of mysql_insert_id
    /// return type is my_ulonglong that realy unsigned long long
    size_t last_insert_id();
    
    class Exception : public std::runtime_error
    {
    public:
        Exception(const std::string& what) :
            std::runtime_error(what)
        {}
    };
    
    class ExCannotGetData : public Exception
    {
    public:
        ExCannotGetData(const std::string& what = "") :
            Exception(std::string("ExCannotGetData: ") + what)
        {}
    };
    
    class ExNoDataReturned : public ExCannotGetData
    {
    public:
        ExNoDataReturned(const std::string& what = "") :
            ExCannotGetData(std::string("ExNoDataReturned: ") + what)
        {}
    };
    
    class ExInvalidIndexes : public ExCannotGetData
    {
    public:
        ExInvalidIndexes(const std::string& what = "") :
            ExCannotGetData(std::string("ExInvalidIndexes: ") + what)
        {}
    };
    
    class ExCannotInitMysql : public Exception
    {
    public:
        ExCannotInitMysql(const std::string& what = "") :
            Exception(std::string("ExCannotInitMysql: ") + what)
        {}
    };

    class ExCannotOpenConnection : public Exception
    {
    public:
        ExCannotOpenConnection(const std::string& what = "") :
            Exception(std::string("ExCannotOpenConnection: ") + what)
        {}
    };

    class ExCannotCompleteQuery : public Exception
    {
    public:
        ExCannotCompleteQuery(const std::string& what = "") :
            Exception(std::string("ExCannotCompleteQuery: ") + what)
        {}
    };

    class ExConnectionAlreadyEstablished : public Exception
    {
    public:
        ExConnectionAlreadyEstablished(const std::string& what = "Connection already established") :
            Exception(std::string("ExConnectionAlreadyEstablished: ") + what)
        {}
    };
    
private:
    MYSQL *connection = nullptr;
    
    MYSQL_RES *m_pResult = nullptr;
    MYSQL_ROW m_currentRow;
    unsigned int m_fieldsCount;
    unsigned long *m_currentLengths = nullptr;
    
    void throwGetException(unsigned int column);
    bool getQueryResult();
    static std::mutex initMutex;
};

template<typename T>
class BlobData
{
    char *chunk;
    int size;
public:    
    BlobData(const T *data, int n)
    {
        if (n == 0) {
            chunk = new char[1];
            chunk[0] = '\0';
            return;
        }
        size = n*sizeof(T);
        chunk = new char[2*size+1];
        //mysql_real_escape_string(Mysql::getConnection(), chunk,
        //                         reinterpret_cast<const char*>(data), size);
        if (size == 0)
            *chunk = '\0';
        else
            mysql_escape_string(chunk, reinterpret_cast<const char*>(data), size);
    }

    virtual ~BlobData()
    {
        delete []chunk;
    }

    const char *getChar()
    {
        return chunk;
    }

    const std::string getString()
    {
        return std::string(chunk);
    }
    
    static void extractArray(T* where, const char* from, int count)
    {
        memcpy(where, from, count*sizeof(T));
    }
};

#endif // MYSQL_WRAPPER_H_INCLUDED
