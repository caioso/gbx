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

shared_ptr<DebugCommand> MessageHandler::ParseReadRegisterCommand(shared_ptr<DebugMessage> message)
{
    // Move this to a "Parse Method" in the ReadRegisterCommand class
    auto targetRegister = (*message->Buffer())[2];

    auto validRegisters = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L,
                        Register::A, Register::F, Register::IR, Register::PIR, Register::R, Register::PC,
                        Register::SP, Register::IX, Register::IY,  Register::HL, Register::BC, Register::DE, Register::AF};

    for (auto reg : validRegisters)
        if (static_cast<uint8_t>(reg) == targetRegister)
        {
            auto command = make_shared<ReadRegisterCommand>(static_cast<gbxcore::interfaces::Register>(targetRegister));
            return static_pointer_cast<DebugCommand>(command);
        }

    throw MessageHandlerException("Invalid target register found when parsing 'ReadRegister' command");
}

void MessageHandler::ProcessMessages(shared_ptr<Runtime> runtime)
{
    while(_commandQueue.size() != 0)
    {
        auto command = _commandQueue.front();

        switch(command->Type())
        {
            case CommandID::CommandReadRegister: runtime->ReadRegister(static_pointer_cast<ReadRegisterCommand>(command)->RegisterToRead()); 
                 break;
        }

        _commandQueue.pop();
    }
}

size_t MessageHandler::Pending()
{
    return _commandQueue.size();
}

}