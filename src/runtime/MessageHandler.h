#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <queue>

#include "../GBXExceptions.h"
#include "../interfaces/DebugCommand.h"
#include "../interfaces/DebugMessage.h"
#include "../interfaces/ServerTransport.h"

#include "../protocol/CommandID.h"
#include "../protocol/MessageID.h"
#include "../protocol/ReadRegisterCommand.h"

#include "DebugMessageNotificationArguments.h"
#include "interfaces/Runtime.h"
#include "Observer.h"

namespace gbx::runtime
{

class MessageHandler : public gbxcommons::Observer, 
                       public std::enable_shared_from_this<MessageHandler>
{
public:
    MessageHandler(std::shared_ptr<interfaces::ServerTransport>);
    ~MessageHandler() = default;
    
    void Initialize();
    size_t Pending();
    void ProcessMessages(std::shared_ptr<gbxcore::interfaces::Runtime>);

    virtual void Notify(std::shared_ptr<gbxcommons::NotificationArguments>) override;

private:
    void ParseMessage(std::shared_ptr<interfaces::DebugMessage>);
    std::shared_ptr<interfaces::DebugCommand> ParseReadRegisterCommand(std::shared_ptr<interfaces::DebugMessage>);

    std::shared_ptr<interfaces::ServerTransport> _transport;
    std::queue<std::shared_ptr<interfaces::DebugCommand>> _commandQueue;
};

}