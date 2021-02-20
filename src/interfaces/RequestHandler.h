#pragma once

#include <memory>

#include "DebugRequest.h"
#include "DebugResponse.h"
#include "interfaces/Runtime.h"

namespace gbx::interfaces
{

class RequestHandler
{
public:
    virtual std::shared_ptr<interfaces::DebugResponse> Process(std::shared_ptr<DebugRequest>, std::shared_ptr<interfaces::Runtime>) = 0;
};

}