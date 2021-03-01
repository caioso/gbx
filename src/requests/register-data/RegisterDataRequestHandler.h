#pragma once

#include <memory>

#include "../../interfaces/DebugMessage.h"
#include "../../interfaces/MessageHandler.h"
#include "../../GBXExceptions.h"
#include "interfaces/RegisterBankInterface.h"
#include "RegisterDataRequest.h"
#include "RegisterDataResponse.h"

namespace gbx::requests
{

class RegisterDataRequestHandler : public interfaces::MessageHandler
{
public:
    RegisterDataRequestHandler() = default;
    virtual ~RegisterDataRequestHandler() = default;

    [[nodiscard]] virtual std::shared_ptr<interfaces::DebugMessage> Process(std::shared_ptr<interfaces::DebugMessage>, std::shared_ptr<gbxcore::interfaces::Runtime>) override;
};

}