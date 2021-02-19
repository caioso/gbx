#pragma once

#include <memory>

#include "DebugRequest.h"
#include "DebugRequestProducer.h"

namespace gbx::interfaces
{

class DebugRequestConsumer
{
public:
    DebugRequestConsumer(std::shared_ptr<DebugRequestProducer>);
    virtual void ConsumeRequest(std::shared_ptr<interfaces::DebugRequest>) = 0;

protected:
    std::shared_ptr<DebugRequestProducer> _producer;
};

}