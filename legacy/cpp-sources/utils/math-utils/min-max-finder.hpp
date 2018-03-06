/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef MIN_MAX_FINDER_H_INCLUDED
#define MIN_MAX_FINDER_H_INCLUDED

#include <exception>

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

template <class T>
class MinMaxFinder
{
public:
    void clear() { isInitialised = false; }

    void add(const T& value)
    {
        if (isInitialised) {
            if (value > m_max) m_max = value;
            else if (value < m_min) m_min = value;
        } else {
            m_max = m_min = value;
            isInitialised = true;
        }
    }
    
    T& getMax()
    {
        if (isInitialised) return m_max;
        else throw ExMinMaxNotInitialised();
    }
    
    T& getMin()
    {
        if (isInitialised) return m_min;
        else throw ExMinMaxNotInitialised();
    }
    
    MinMaxFinder() {}
    
    ///////////////////////
    // Exceptions    
    class Exception : public std::exception {};
    
    class ExMinMaxNotInitialised : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Min and max values not initialised but requested"; }
    };
    
private:
    T m_max, m_min;
    bool isInitialised = false;
};


/** @brief MinMax finder, but min and max value can be accompanied with any kind of data. For example T is value, AttachedType is its index
 */
template <typename T, typename AttachedType>
class MinMaxFinderWithData
{
public:
    void clear() { isInitialised = false; }

    void add(const T& value, const AttachedType& attached)
    {
        if (isInitialised) {
            if (value > m_max)
            {
                m_max = value;
                m_maxAttached = attached;
            }
            else if (value < m_min) 
            {
                m_min = value;
                m_minAttached = attached;
            }
        } else {
            m_max = m_min = value;
            m_maxAttached = m_minAttached = attached;
            isInitialised = true;
        }
    }
    
    T& getMax()
    {
        throwIfNotInitialized();
        return m_max;
    }
    
    T& getMin()
    {
        throwIfNotInitialized();
        return m_min;
    }
    
    AttachedType& getMaxAttached()
    {
        throwIfNotInitialized();
        return m_maxAttached;
    }
    
    AttachedType& getMinAttached()
    {
        throwIfNotInitialized();
        return m_minAttached;
    }
    
    ///////////////////////
    // Exceptions    
    class Exception : public std::exception {};
    
    class ExMinMaxNotInitialised : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Min and max values not initialised but requested"; }
    };
    
private:
    void throwIfNotInitialized()
    {
        if (!isInitialised) throw ExMinMaxNotInitialised();
    }
    
    T m_max, m_min;
    AttachedType m_maxAttached, m_minAttached;
    bool isInitialised = false;
};

template <class ArgumentType, class ResultType>
class MinMaxFuncFinder
{
public:
    typedef ResultType (*FuncToMinMax)(const ArgumentType&);
    void clear() { isInitialised = false; }

    void add(const ArgumentType& value)
    {
        ResultType result = (*m_pFunc)(value);
        if (isInitialised) {
            if (result > m_maxVal) {
                m_max = value;
                m_maxVal = result;
            }
            else if (result < m_minVal) {
                m_min = value;
                m_minVal = result;
            }
        } else {
            m_max = m_min = value;
            m_maxVal = m_minVal = result;
            isInitialised = true;
        }
    }
    
    ArgumentType& getMax()
    {
        throwIfNotInitialized(); return m_max;
    }
    
    ResultType getMaxFuncValue()
    {
        throwIfNotInitialized(); return m_maxVal;
    }
    
    ArgumentType& getMin()
    {
        throwIfNotInitialized(); return m_min;
    }
    
    ResultType getMinFuncValue()
    {
        throwIfNotInitialized(); return m_maxVal;
    }
    
    MinMaxFuncFinder(FuncToMinMax func) : m_pFunc(func), isInitialised(false) {}
    
    ///////////////////////
    // Exceptions    
    class Exception : public std::exception {};
    
    class ExMinMaxNotInitialised : public Exception
    {
    public:        
        virtual const char* what() const throw() { return "Min and max values not initialised but requested"; }
    };
    
private:
    void throwIfNotInitialized()
    {
        if (!isInitialised) throw ExMinMaxNotInitialised();
    }
    
    FuncToMinMax m_pFunc;
    ArgumentType m_max, m_min;
    ResultType m_maxVal, m_minVal;
    bool isInitialised;
};

//#pragma GCC diagnostic pop

#endif // MIN_MAX_FINDER_H_INCLUDED
