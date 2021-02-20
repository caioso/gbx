#pragma once

#include <memory>

#include "DebugRequest.h"


namespace gbx::interfaces
{

class DebugRequestProducer;

class DebugRequestConsumer
{
public:
    DebugRequestConsumer(std::shared_ptr<interfaces::DebugRequestProducer>);
    virtual void ConsumeRequest(std::shared_ptr<interfaces::DebugRequest>) = 0;

protected:
    std::shared_ptr<DebugRequestProducer> _producer;
};

}