#pragma once

#include <memory>

#include "../interfaces/RequestHandler.h"
#include "../GBXExceptions.h"
#include "interfaces/RegisterBankInterface.h"
#include "RegisterDataRequest.h"
#include "RegisterDataResponse.h"

namespace gbx::requests
{

class RegisterDataRequestHandler : public interfaces::RequestHandler
{
public:
    RegisterDataRequestHandler() = default;
    virtual ~RegisterDataRequestHandler() = default;

    [[nodiscard]] virtual std::shared_ptr<interfaces::DebugResponse> Process(std::shared_ptr<interfaces::DebugRequest>, std::shared_ptr<interfaces::Runtime>) override;
};

}