#pragma once

#include <memory>

#include "../interfaces/RequestHandler.h"

namespace gbx::requests
{

class RegisterDataRequestHandler : public interfaces::RequestHandler
{
public:
    RegisterDataRequestHandler() = default;
    virtual ~RegisterDataRequestHandler() = default;

    virtual std::shared_ptr<interfaces::DebugResponse> Process(std::shared_ptr<interfaces::DebugRequest>, std::shared_ptr<interfaces::Runtime>) override;
};

}