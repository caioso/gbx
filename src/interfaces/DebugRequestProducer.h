#pragma once

#include <memory>

#include "DebugResponse.h"
#include "DebugRequestConsumer.h"

namespace gbx::interfaces
{

class DebugRequestProducer
{
public:
    virtual void ConsumeResponse(std::shared_ptr<interfaces::DebugResponse>) = 0;
};

}