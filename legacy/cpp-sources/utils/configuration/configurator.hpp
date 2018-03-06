/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */
 
#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include "macros.hpp"
#include "logging.hpp"
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>

class Configurator
{
public:
    void readConfig(const std::string& filename);
    
    template <class T>
    T get(std::string parameter)
    {
        T result;
        try {
            result = m_pt.get<T>(parameter);
        }
        catch(boost::property_tree::ptree_error &exception)
        {
            throw ATT_SCOPES_STACK(ExConfiguratorFailed(std::string("Config parsing error: ") + exception.what()));
        }
        catch(...)
        {
            throw ATT_SCOPES_STACK(ExConfiguratorFailed(std::string("Unknown configuration parsing error")));
        }
        return result;
    }
    
    template <class T>
    T get(std::string parameter, T defaultValue)
    {
        T result;
        try {
            result = m_pt.get<T>(parameter, defaultValue);
        }
        catch(boost::property_tree::ptree_error &exception)
        {
            throw ATT_SCOPES_STACK(ExConfiguratorFailed(std::string("Parsing error: ") + exception.what()));
        }
        catch(...)
        {
            throw ATT_SCOPES_STACK(ExConfiguratorFailed("Config reading error"));
        }
        return result;
    }
    
    const boost::property_tree::ptree& tree() { return m_pt; }

    WRAP_STDEXCEPT(ExConfiguratorFailed, std::runtime_error)
    
    SIGLETON_IN_CLASS(Configurator);

private:
    Configurator();
    boost::property_tree::ptree m_pt;
};

#endif //CONFIGURATOR_H
