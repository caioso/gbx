#include "MessageHandler.h"

using namespace gbx;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
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
    _transport->Subscribe(static_pointer_cast<Observer>(shared_from_this()));
}

void MessageHandler::Notify(shared_ptr<NotificationArguments> args)
{
    auto debugMessageArgs = static_pointer_cast<DebugMessageNotificationArguments>(args);
    ParseMessage(debugMessageArgs->Message());
}

void MessageHandler::ParseMessage(shared_ptr<DebugMessage> messagePointer)
{
    uint16_t messageID = (*messagePointer->Buffer())[0] | ((*messagePointer->Buffer())[1] << 0x08); 
    
    switch (messageID)
    {
        // Parse messages and add them to the queue.
        case MessageID::MessageClientJoined: _commandQueue.push(ParseClientJoinedCommand(messagePointer));
             break;
        case MessageID::MessageReadRegister: _commandQueue.push(ParseReadRegisterCommand(messagePointer));
             break;
        case MessageID::MessageRegisterBankSummary: _commandQueue.push(ParseRegisterBankSummaryCommand(messagePointer));
             break;
        case MessageID::MessageWriteRegister: _commandQueue.push(ParseWriteRegisterCommand(messagePointer));
             break;
        default:
            // Send an error command back here!!!!!
            throw MessageHandlerException("Invalid debug message recieved and will be ignored");
    }
}

void MessageHandler::ProcessMessages(shared_ptr<Runtime> runtime, shared_ptr<DebuggableRunner> runner)
{
    while(_commandQueue.size() != 0)
    {
        auto command = _commandQueue.front();
        shared_ptr<DebugMessage> response = nullptr;

        switch(command->Type())
        {
            case CommandID::CommandClientJoined: RunClientJoinedCommand(runner); break;
            case CommandID::CommandReadRegister: response = RunReadRegisterCommand(command, runtime); break;
            case CommandID::CommandRegisterBankSummary: response = RunRegisterBankSummaryCommand(command, runtime); break;
            case CommandID::CommandWriteRegister: response = RunWriteRegisterCommand(command, runtime); break;
            case CommandID::CommandError: response = RunErrorCommand(command); break;
        }

        if (response != nullptr)
            _transport->SendMessage(response);

        _commandQueue.pop();
    }
}

shared_ptr<DebugCommand> MessageHandler::ParseReadRegisterCommand(shared_ptr<DebugMessage> message)
{
    auto readRegisterCommand = make_shared<ReadRegisterCommand>();
    
    try
    {
        readRegisterCommand->DecodeRequestMessage(message);
    }
    catch (const MessageHandlerException& e)
    {
        return make_shared<ErrorCommand>(ErrorID::InvalidRegister);
    }

    return readRegisterCommand;
}

shared_ptr<DebugCommand> MessageHandler::ParseClientJoinedCommand(shared_ptr<DebugMessage> message)
{
    auto clientJoinedCommand = make_shared<ClientJoinedCommand>();
    clientJoinedCommand->DecodeRequestMessage(message);
    return clientJoinedCommand;
}

shared_ptr<DebugCommand> MessageHandler::ParseRegisterBankSummaryCommand(shared_ptr<DebugMessage> message)
{
    auto registerBankSummaryCommand = make_shared<RegisterBankSummaryCommand>();
    registerBankSummaryCommand->DecodeRequestMessage(message);
    return registerBankSummaryCommand;
}

shared_ptr<DebugCommand> MessageHandler::ParseWriteRegisterCommand(shared_ptr<DebugMessage> message)
{
    auto writeRegisterCommand = make_shared<WriteRegisterCommand>();

    try
    {
        writeRegisterCommand->DecodeRequestMessage(message);
    }
    catch (const MessageHandlerException& e)
    {
        return make_shared<ErrorCommand>(ErrorID::InvalidRegister);
    }

    return writeRegisterCommand;
}

void MessageHandler::RunClientJoinedCommand(shared_ptr<DebuggableRunner> runner)
{
    runner->ClientJoined();
}

shared_ptr<DebugMessage> MessageHandler::RunErrorCommand(shared_ptr<DebugCommand> command)
{
    return static_pointer_cast<ErrorCommand>(command)->EncodeRequestMessage();
}

shared_ptr<DebugMessage> MessageHandler::RunReadRegisterCommand(shared_ptr<DebugCommand> command, shared_ptr<Runtime> runtime)
{
    auto valueVariant = runtime->ReadRegister(static_pointer_cast<ReadRegisterCommand>(command)->RegisterToRead());
    auto value = holds_alternative<uint16_t>(valueVariant) ? get<uint16_t>(valueVariant) : static_cast<uint16_t>(get<uint8_t>(valueVariant));
    static_pointer_cast<ReadRegisterCommand>(command)->SetRegisterValue(value);
    return command->EncodeRequestMessage();
}

shared_ptr<DebugMessage> MessageHandler::RunRegisterBankSummaryCommand(shared_ptr<DebugCommand> command, shared_ptr<gbxcore::interfaces::Runtime> runtime)
{
    static_pointer_cast<RegisterBankSummaryCommand>(command)->GenerateSummary(runtime);
    return command->EncodeRequestMessage();
}

shared_ptr<DebugMessage> MessageHandler::RunWriteRegisterCommand(shared_ptr<DebugCommand> command, shared_ptr<gbxcore::interfaces::Runtime> runtime)
{
    auto valueVariant = static_pointer_cast<WriteRegisterCommand>(command)->RegisterValue();
    auto targetRegister = static_pointer_cast<WriteRegisterCommand>(command)->Register();
    runtime->WriteRegister(targetRegister, valueVariant);
    return command->EncodeRequestMessage();
}

size_t MessageHandler::Pending()
{
    return _commandQueue.size();
}

}