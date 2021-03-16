#pragma once

#include <memory>

#include "DebugMessage.h"
#include "Observer.h"

namespace gbxdb::protocol
{
class DebugMessageNotificationArguments : public gbxcommons::NotificationArguments
{
public:
    DebugMessageNotificationArguments(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    ~DebugMessageNotificationArguments() = default;
    
    std::shared_ptr<gbxdb::interfaces::DebugMessage> Message();

private:
    std::shared_ptr<gbxdb::interfaces::DebugMessage> _debugMessage;
};

}