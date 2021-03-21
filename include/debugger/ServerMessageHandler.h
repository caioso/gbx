#pragma once

#include <array>
#include <memory>
#include <queue>
#include <variant>

#include "GBXDebuggerExceptions.h"
#include "DebugCommand.h"
#include "DebuggableRunner.h"
#include "DebugMessage.h"
#include "ServerTransport.h"

#include "ErrorID.h"
#include "ErrorCommand.h"

#include "ClientJoinedCommand.h"
#include "ReadRegisterCommand.h"
#include "RegisterBankSummaryCommand.h"
#include "CommandID.h"
#include "MessageID.h"
#include "WriteRegisterCommand.h"

#include "DebugMessageNotificationArguments.h"
#include "Runtime.h"
#include "Observer.h"

namespace gbxdb
{

class ServerMessageHandler : public gbxcommons::Observer, 
                             public std::enable_shared_from_this<ServerMessageHandler>
{
public:
    ServerMessageHandler(std::shared_ptr<gbxdb::interfaces::ServerTransport>);
    ~ServerMessageHandler() = default;
    
    void Initialize();
    size_t Pending();
    void ProcessMessages(std::shared_ptr<gbxcore::interfaces::Runtime>, std::shared_ptr<gbxdb::interfaces::DebuggableRunner>);

    void Notify(std::shared_ptr<gbxcommons::NotificationArguments>) override;

private:
    void ParseMessage(std::shared_ptr<gbxdb::interfaces::DebugMessage>);

    std::shared_ptr<gbxdb::interfaces::DebugCommand> ParseReadRegisterCommand(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    std::shared_ptr<gbxdb::interfaces::DebugCommand> ParseClientJoinedCommand(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    std::shared_ptr<gbxdb::interfaces::DebugCommand> ParseRegisterBankSummaryCommand(std::shared_ptr<gbxdb::interfaces::DebugMessage>);
    std::shared_ptr<gbxdb::interfaces::DebugCommand> ParseWriteRegisterCommand(std::shared_ptr<gbxdb::interfaces::DebugMessage>);

    void RunClientJoinedCommand(std::shared_ptr<gbxdb::interfaces::DebuggableRunner>);
    [[nodiscard]] std::shared_ptr<gbxdb::interfaces::DebugMessage> RunErrorCommand(std::shared_ptr<gbxdb::interfaces::DebugCommand>);

    [[nodiscard]] std::shared_ptr<gbxdb::interfaces::DebugMessage> RunReadRegisterCommand(std::shared_ptr<gbxdb::interfaces::DebugCommand>, std::shared_ptr<gbxcore::interfaces::Runtime>);
    [[nodiscard]] std::shared_ptr<gbxdb::interfaces::DebugMessage> RunRegisterBankSummaryCommand(std::shared_ptr<gbxdb::interfaces::DebugCommand>, std::shared_ptr<gbxcore::interfaces::Runtime>);
    [[nodiscard]] std::shared_ptr<gbxdb::interfaces::DebugMessage> RunWriteRegisterCommand(std::shared_ptr<gbxdb::interfaces::DebugCommand>, std::shared_ptr<gbxcore::interfaces::Runtime>);

    std::shared_ptr<gbxdb::interfaces::ServerTransport> _transport;
    std::queue<std::shared_ptr<gbxdb::interfaces::DebugCommand>> _commandQueue;
};

}