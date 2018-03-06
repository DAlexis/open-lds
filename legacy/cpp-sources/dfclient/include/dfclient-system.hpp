/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef DFCLIENT_SYSTEM_H
#define DFCLIENT_SYSTEM_H

#include "receiver-controller.hpp"
#include "configurator.hpp"
#include "logging.hpp"

class DFClientSystem
{
public:
    void init(const std::string& configFileName);
    void stop();
    void join();
    
    DFClientSystem();
    ~DFClientSystem();
    
    static void setupLogger();

    WRAP_STDEXCEPT(ExInitFailed, std::runtime_error)
    WRAP_STDEXCEPT(ExRuntimeError, std::runtime_error)

private:
    DataCollector collector;
    ReceiverController receiver;
};

#endif //DFCLIENT_SYSTEM_H
