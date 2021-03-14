#pragma once

#include <memory>

#include "../interfaces/DebugMessage.h"
#include "Observer.h"

namespace gbx::runtime
{

class DebugMessageNotificationArguments : public gbxcommons::NotificationArguments
{
public:
    DebugMessageNotificationArguments(std::shared_ptr<interfaces::DebugMessage>);
    ~DebugMessageNotificationArguments() = default;
    
    std::shared_ptr<interfaces::DebugMessage> Message();

private:
    std::shared_ptr<interfaces::DebugMessage> _debugMessage;
};

}