#pragma once

#include <array>
#include <memory>
#include <queue>
#include <variant>

#include "GBXEmulatorExceptions.h"
#include "DebugCommand.h"
#include "DebuggableRunner.h"
#include "DebugMessage.h"
#include "ServerTransport.h"

#include "CommandID.h"
#include "ErrorID.h"
#include "MessageID.h"
#include "ErrorCommand.h"
#include "ReadRegisterCommand.h"
#include "ClientJoinedCommand.h"
#include "RegisterBankSummaryCommand.h"
#include "WriteRegisterCommand.h"

#include "DebugMessageNotificationArguments.h"
#include "Runtime.h"
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
    void ProcessMessages(std::shared_ptr<gbxcore::interfaces::Runtime>, std::shared_ptr<interfaces::DebuggableRunner>);

    void Notify(std::shared_ptr<gbxcommons::NotificationArguments>) override;

private:
    void ParseMessage(std::shared_ptr<interfaces::DebugMessage>);

    std::shared_ptr<interfaces::DebugCommand> ParseReadRegisterCommand(std::shared_ptr<interfaces::DebugMessage>);
    std::shared_ptr<interfaces::DebugCommand> ParseClientJoinedCommand(std::shared_ptr<interfaces::DebugMessage>);
    std::shared_ptr<interfaces::DebugCommand> ParseRegisterBankSummaryCommand(std::shared_ptr<interfaces::DebugMessage>);
    std::shared_ptr<interfaces::DebugCommand> ParseWriteRegisterCommand(std::shared_ptr<interfaces::DebugMessage>);

    void RunClientJoinedCommand(std::shared_ptr<interfaces::DebuggableRunner>);
    [[nodiscard]] std::shared_ptr<interfaces::DebugMessage> RunErrorCommand(std::shared_ptr<interfaces::DebugCommand>);

    [[nodiscard]] std::shared_ptr<interfaces::DebugMessage> RunReadRegisterCommand(std::shared_ptr<interfaces::DebugCommand>, std::shared_ptr<gbxcore::interfaces::Runtime>);
    [[nodiscard]] std::shared_ptr<interfaces::DebugMessage> RunRegisterBankSummaryCommand(std::shared_ptr<interfaces::DebugCommand>, std::shared_ptr<gbxcore::interfaces::Runtime>);
    [[nodiscard]] std::shared_ptr<interfaces::DebugMessage> RunWriteRegisterCommand(std::shared_ptr<interfaces::DebugCommand>, std::shared_ptr<gbxcore::interfaces::Runtime>);

    std::shared_ptr<interfaces::ServerTransport> _transport;
    std::queue<std::shared_ptr<interfaces::DebugCommand>> _commandQueue;
};

}