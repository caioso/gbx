#pragma once

#include <memory>

#include "DebugMessage.h"
#include "interfaces/Runtime.h"

namespace gbx::interfaces
{

class MessageHandler
{
public:
    [[nodiscard]] virtual std::shared_ptr<interfaces::DebugMessage> Process(std::shared_ptr<interfaces::DebugMessage>, std::shared_ptr<gbxcore::interfaces::Runtime>) = 0;
};

}