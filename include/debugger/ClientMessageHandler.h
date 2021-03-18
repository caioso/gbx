#pragma once

#include <array>
#include <memory>
#include <vector>

#include "ClientTransport.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXDebuggerExceptions.h"
#include "Observer.h"

#include "ClientMessageID.h"
#include "ClientJoinedServerCommand.h"

namespace gbxdb
{

class ClientMessageHandler : public std::enable_shared_from_this<ClientMessageHandler>
                           , public gbxcommons::Observer
{
public:
    ClientMessageHandler(std::shared_ptr<gbxdb::interfaces::ClientTransport>);
    ~ClientMessageHandler() = default;

    void Initialize();
    void Notify(std::shared_ptr<gbxcommons::NotificationArguments>) override;

    bool IsConnected();

private:
    void ParseMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>);

    void HandleJoinedServerCommand(std::shared_ptr<gbxdb::interfaces::DebugMessage>);

    std::shared_ptr<gbxdb::interfaces::ClientTransport> _transport;
    bool _isConnected{};
};

}