/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef RECEIVER_CONTROLLER_H
#define RECEIVER_CONTROLLER_H

#include "data-collector.hpp"
#include "utils.hpp"
#include "configurator.hpp"
#include "hardware-interface.hpp"
#include "boltek-hw.hpp"
#include <memory>

/// This class manage all job related to hardware device
class ReceiverController : public LoopFuncHolder,
                              public DeviceAndExperimentSpecifiedClass
{
public:
    ReceiverController(DataCollector& collector);
    virtual ~ReceiverController();
    
    /// Get configuration and initialize hardware
    void init();
    
    /// Deinitialize hardware
    void closeDevice();
    
    void stopAsync();

    void setSquelch(unsigned int squelch);
    
    class ExCannotInitBoard : public std::exception
    {
    public:
        virtual const char* what() const throw() { return "Cannot init PCI board. Check kernel module to be loaded!"; }
    };

private:
    virtual void loopFunction();
    std::unique_ptr<ILightningDetectionHardware> m_lightningDetector{nullptr};
    unsigned int m_squelch = 0;
        
    DataCollector *m_pDataCollector;
};

#endif //RECEIVER_CONTROLLER_H
