/*
 * Copyright (c) 2013 Institute of Applied Physics of the Russian Academy of Sciences
 */

#ifndef DATA_COLLECTOR_INCLUDED
#define DATA_COLLECTOR_INCLUDED


#include "MysqlWrapper.hpp"
#include "strike-data-container-interface.hpp"
#include "output-provider-interface.hpp"
#include "stormpci.h"
#include "output-manager.hpp"
#include "utils.hpp"
#include <queue>
#include <mutex>


/// This class manage queue of data to be outputted
class DataCollector : public LoopFuncHolder
{
public:
    DataCollector();
    virtual ~DataCollector();
    
    /// Get confuguration
    void init();
    
    void stopAsync();

    void collectData(std::shared_ptr<const IStrikeDataContainer> pstrike);
    void registerOutputProvider(IOutputProvider* provider);
    
protected:
    
    enum SendDataRusult {
        SR_OK = 0,
        SR_FAILED
    };

    struct OutputContext
    {
    	std::mutex queueMutex;
    	std::queue<std::shared_ptr<const IStrikeDataContainer>> queue;
    	std::unique_ptr<IOutputProvider> provider;
    };

    virtual void loopFunction();

    std::vector<std::unique_ptr<OutputContext>> m_outputContexts;
};

#endif //DATA_COLLECTOR_INCLUDED
