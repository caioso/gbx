#include "ServerMessageHandler.h"

using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcommons;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxdb
{

ServerMessageHandler::ServerMessageHandler(unique_ptr<ServerTransport> transport)
    : _transport(std::move(transport))
{}

void ServerMessageHandler::Initialize()
{
    _transport->Subscribe(shared_from_this());
    _transport->WaitForClient();
    _transport->InitializeProtocol();
}

void ServerMessageHandler::Notify(shared_ptr<NotificationArguments> args)
{
    auto debugMessageArgs = static_pointer_cast<DebugMessageNotificationArguments>(args);
    ParseMessage(debugMessageArgs->Message());
}

void ServerMessageHandler::ParseMessage(shared_ptr<DebugMessage> messagePointer)
{
    uint16_t messageID = (*messagePointer->Buffer())[0] | ((*messagePointer->Buffer())[1] << 0x08); 
    cout << hex << static_cast<size_t>(messageID) << '\n';
    switch (messageID)
    {
        // Parse messages and add them to the queue.
        case MessageID::MessageJoined: _commandQueue.push(ParseClientJoinedCommand(messagePointer));
             break;
        case MessageID::MessageReadRegister: _commandQueue.push(ParseReadRegisterCommand(messagePointer));
             break;
        case MessageID::MessageRegisterBankSummary: _commandQueue.push(ParseRegisterBankSummaryCommand(messagePointer));
             break;
        case MessageID::MessageWriteRegister: _commandQueue.push(ParseWriteRegisterCommand(messagePointer));
             break;
        case MessageID::MessageProtocolInitializer: _commandQueue.push(ParseProtocolInitializerCommand(messagePointer));
             break;
        default:
            // Send an error command back here!!!!!
            throw MessageHandlerException("Invalid debug message recieved and will be ignored");
    }
}

void ServerMessageHandler::ProcessMessages(shared_ptr<Runtime> runtime, shared_ptr<DebuggableRunner> runner)
{
    while(_commandQueue.size() != 0)
    {
        auto command = _commandQueue.front();
        shared_ptr<DebugMessage> response = nullptr;

        switch(command->Type())
        {
            case CommandID::CommandJoined: RunClientJoinedCommand(runner); break;
            case CommandID::CommandProtocolInitializer: response = RunProtocolInitializerCommand(command); break;
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

shared_ptr<DebugCommand> ServerMessageHandler::ParseProtocolInitializerCommand(shared_ptr<DebugMessage> message)
{
    cout << "Parse Protocol Initializer" << '\n';
    auto protocolInitializer = make_shared<ProtocolInitializerCommand>();
    protocolInitializer->SetProtocolInitializationParameters(*message->Buffer());
    auto protocolInitializerCommand = make_shared<ProtocolInitializerCommand>();
    return protocolInitializerCommand;
}

shared_ptr<DebugCommand> ServerMessageHandler::ParseReadRegisterCommand(shared_ptr<DebugMessage> message)
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

shared_ptr<DebugCommand> ServerMessageHandler::ParseClientJoinedCommand(shared_ptr<DebugMessage> message)
{
    auto clientJoinedCommand = make_shared<ClientJoinedCommand>();
    clientJoinedCommand->DecodeRequestMessage(message);
    return clientJoinedCommand;
}

shared_ptr<DebugCommand> ServerMessageHandler::ParseRegisterBankSummaryCommand(shared_ptr<DebugMessage> message)
{
    auto registerBankSummaryCommand = make_shared<RegisterBankSummaryCommand>();
    registerBankSummaryCommand->DecodeRequestMessage(message);
    return registerBankSummaryCommand;
}

shared_ptr<DebugCommand> ServerMessageHandler::ParseWriteRegisterCommand(shared_ptr<DebugMessage> message)
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

shared_ptr<DebugMessage> ServerMessageHandler::RunProtocolInitializerCommand(shared_ptr<DebugCommand> command)
{
    return command->EncodeCommandMessage();
}

void ServerMessageHandler::RunClientJoinedCommand(shared_ptr<DebuggableRunner> runner)
{
    runner->ClientJoined();
}

shared_ptr<DebugMessage> ServerMessageHandler::RunErrorCommand(shared_ptr<DebugCommand> command)
{
    return static_pointer_cast<ErrorCommand>(command)->EncodeCommandMessage();
}

shared_ptr<DebugMessage> ServerMessageHandler::RunReadRegisterCommand(shared_ptr<DebugCommand> command, shared_ptr<Runtime> runtime)
{
    auto valueVariant = runtime->ReadRegister(static_pointer_cast<ReadRegisterCommand>(command)->RegisterToRead());
    auto value = holds_alternative<uint16_t>(valueVariant) ? get<uint16_t>(valueVariant) : static_cast<uint16_t>(get<uint8_t>(valueVariant));
    static_pointer_cast<ReadRegisterCommand>(command)->SetRegisterValue(value);
    return command->EncodeCommandMessage();
}

shared_ptr<DebugMessage> ServerMessageHandler::RunRegisterBankSummaryCommand(shared_ptr<DebugCommand> command, shared_ptr<gbxcore::interfaces::Runtime> runtime)
{
    static_pointer_cast<RegisterBankSummaryCommand>(command)->SetSummary(runtime);
    return command->EncodeCommandMessage();
}

shared_ptr<DebugMessage> ServerMessageHandler::RunWriteRegisterCommand(shared_ptr<DebugCommand> command, shared_ptr<gbxcore::interfaces::Runtime> runtime)
{
    auto valueVariant = static_pointer_cast<WriteRegisterCommand>(command)->RegisterValue();
    auto targetRegister = static_pointer_cast<WriteRegisterCommand>(command)->Register();
    runtime->WriteRegister(targetRegister, valueVariant);
    return command->EncodeCommandMessage();
}

size_t ServerMessageHandler::Pending()
{
    return _commandQueue.size();
}

}