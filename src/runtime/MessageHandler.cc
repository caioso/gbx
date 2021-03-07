#include "MessageHandler.h"

using namespace gbx::interfaces;
using namespace gbx::protocol;
using namespace gbx;
using namespace gbxcommons;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbx::runtime
{

MessageHandler::MessageHandler(shared_ptr<ServerTransport> transport)
    : _transport(transport)
{}

void MessageHandler::Initialize()
{
    _transport->Subscribe(shared_from_this());
}

void MessageHandler::Notify(shared_ptr<NotificationArguments> args)
{
    auto debugMessageArgs = static_pointer_cast<DebugMessageNotificationArguments>(args);
    ParseMessage(debugMessageArgs->Message());
}

void MessageHandler::ParseMessage(std::shared_ptr<interfaces::DebugMessage> messagePointer)
{
    uint16_t messageID = (*messagePointer->Buffer())[0] | ((*messagePointer->Buffer())[1] << 0x08); 
    
    switch (messageID)
    {
        // Parse messages and add them to the queue.
        case MessageID::MessageReadRegister: _commandQueue.push(ParseReadRegisterCommand(messagePointer));
             break;
        default:
            throw MessageHandlerException("Invalid debug message recieved and will be ignored");
    }
}

void MessageHandler::ProcessMessages(shared_ptr<Runtime> runtime)
{
    while(_commandQueue.size() != 0)
    {
        auto command = _commandQueue.front();
        shared_ptr<DebugMessage> response;

        switch(command->Type())
        {
            case CommandID::CommandReadRegister: response = RunReadRegisterCommand(command, runtime);
                 break;
        }

        _transport->SendMessage(response);
        _commandQueue.pop();
    }
}

shared_ptr<DebugCommand> MessageHandler::ParseReadRegisterCommand(shared_ptr<DebugMessage> message)
{
    auto readRegisterCommand = make_shared<ReadRegisterCommand>();
    readRegisterCommand->DecodeRequestMessage(message);
    return readRegisterCommand;
}

std::shared_ptr<interfaces::DebugMessage> MessageHandler::RunReadRegisterCommand(std::shared_ptr<interfaces::DebugCommand> command, shared_ptr<Runtime> runtime)
{
    auto valueVariant = runtime->ReadRegister(static_pointer_cast<ReadRegisterCommand>(command)->RegisterToRead());
    auto value = holds_alternative<uint16_t>(valueVariant) ? get<uint16_t>(valueVariant) : static_cast<uint16_t>(get<uint8_t>(valueVariant));
    static_pointer_cast<ReadRegisterCommand>(command)->SetRegisterValue(value);
    return command->EncodeRequestMessage();
}

size_t MessageHandler::Pending()
{
    return _commandQueue.size();
}

}