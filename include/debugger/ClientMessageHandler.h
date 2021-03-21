#pragma once

#include <array>
#include <memory>
#include <vector>

#include "ClientTransport.h"
#include "DebugMessage.h"
#include "DebugMessageNotificationArguments.h"
#include "GBXDebuggerExceptions.h"
#include "Observer.h"
#include "OutputDriver.h"

#include "MessageID.h"
#include "ClientJoinedCommand.h"
#include "RegisterBankSummaryCommand.h"

namespace gbxdb
{

class ClientMessageHandler : public std::enable_shared_from_this<ClientMessageHandler>
                           , public gbxcommons::Observer
{
public:
    ClientMessageHandler(std::shared_ptr<gbxdb::interfaces::ClientTransport>, gbxdb::output::OutputDriver&);
    ~ClientMessageHandler() = default;

    void Initialize();
    void Notify(std::shared_ptr<gbxcommons::NotificationArguments>) override;

    // From-Command line command handlers
    void SendRegisterBankSummaryMessage();

    bool IsConnected();

private:
    void ParseMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    void HandleJoinedServerCommand(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    void HandleRegisterBankSummaryMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>);

    std::shared_ptr<gbxdb::interfaces::ClientTransport> _transport;
    bool _isConnected{};
    gbxdb::output::OutputDriver& _outputDriver;
};

}