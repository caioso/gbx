#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <random>

#include "DebugMessageNotificationArguments.h"
#include "GBXEmulatorExceptions.h"
#include "MessageID.h"
#include "ServerMessageHandler.h"

#include "DebuggerTestMocks.h"
#include "TestUtils.h"

using namespace gbxruntime;
using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace gbxdb::protocol;
using namespace gbxcore::interfaces;
using namespace gbxcommons;
using namespace std;

shared_ptr<array<uint8_t, MaxMessageBufferSize>> CreateReadRegisterMessage(uint8_t targetRegister)
{
    auto buffer = make_shared<std::array<uint8_t, MaxMessageBufferSize>>();
    (*buffer)[0] = 0xFE;
    (*buffer)[1] = 0xFF;
    (*buffer)[2] = targetRegister;
    return buffer;
}

TEST(DebuggerTests_ReadRegisterServerMessage, ReadRegisterMessage8Bit) 
{
    auto operand8BitList = {Register::B, Register::C, Register::D, Register::E, Register::H, Register::L, Register::A, Register::F };
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto runnerMock = make_shared<DebuggableRunnerMock>();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint8_t> distribution{0x00, 0xFF};

    for (auto operand : operand8BitList)
    {
        auto readRegisterBankMessage = make_shared<DebugMessage>(CreateReadRegisterMessage(static_cast<uint8_t>(operand)));
        auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
        auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

        EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
        EXPECT_CALL((*transportPointer), WaitForClient()); 
        EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);  
        messageHandler->Initialize();

        // Message arrived from the client (stored in the internal queue)
        messageHandler->Notify(argumentsPointer);
        EXPECT_EQ(1llu, messageHandler->Pending());

        // Expect call to Runtime Read Register Method
        auto registerValue = distribution(engine);
        EXPECT_CALL((*runtimeMock), ReadRegister(operand)).WillOnce(Return(registerValue));
        EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
        {
            uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
            uint16_t value = (*(argument->Buffer()))[2] | (*(argument->Buffer()))[3] << 0x08;

            EXPECT_EQ(MessageID::MessageReadRegister, messageId);
            EXPECT_EQ(registerValue, value);
        }));

        // Take message from the queue and process it
        messageHandler->ProcessMessages(runtimeMock, runnerMock);
        EXPECT_EQ(0llu, messageHandler->Pending());
    }
}

TEST(DebuggerTests_ReadRegisterServerMessage, ReadRegisterMessage16Bit) 
{
    auto operand16BitList = {Register::PC, Register::SP, Register::HL, Register::BC, Register::DE, Register::AF};
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    random_device randomDevice;
    mt19937 engine{randomDevice()};
    uniform_int_distribution<uint16_t> distribution{0x00, 0xFFFF};

    for (auto operand : operand16BitList)
    {
        auto readRegisterBankMessage = make_shared<DebugMessage>(CreateReadRegisterMessage(static_cast<uint8_t>(operand)));
        auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
        auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

        EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
        EXPECT_CALL((*transportPointer), WaitForClient());   
        EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
        messageHandler->Initialize();

        messageHandler->Notify(argumentsPointer);
        EXPECT_EQ(1llu, messageHandler->Pending());

        // Expect call to Runtime Read Register Method
        auto registerValue = distribution(engine);
        EXPECT_CALL((*runtimeMock), ReadRegister(operand)).WillOnce(Return(registerValue));
        EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
        {
            // Analyze Encoded message content
            uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
            uint16_t value = (*(argument->Buffer()))[2] | (*(argument->Buffer()))[3] << 0x08;

            EXPECT_EQ(MessageID::MessageReadRegister, messageId);
            EXPECT_EQ(registerValue, value);
        }));

        messageHandler->ProcessMessages(runtimeMock, runnerMock);
        EXPECT_EQ(0llu, messageHandler->Pending());
    }
}

TEST(DebuggerTests_ReadRegisterServerMessage, DecodeReadRegisterUnknownRegister) 
{
    auto transportMock = make_unique<ServerTransportMock>();
    auto transportPointer = transportMock.get();
    auto runtimeMock = make_shared<RuntimeMock>();
    auto messageHandler = make_shared<ServerMessageHandler>(std::move(transportMock));
    auto runnerMock = make_shared<DebuggableRunnerMock>();

    auto readRegisterBankMessage = make_shared<DebugMessage>(CreateReadRegisterMessage(static_cast<uint8_t>(0xFF)));
    auto notificationArguments = make_shared<DebugMessageNotificationArguments>(readRegisterBankMessage);
    auto argumentsPointer = static_pointer_cast<NotificationArguments>(notificationArguments);

    EXPECT_CALL((*transportPointer), Subscribe(::_)).Times(1);
    EXPECT_CALL((*transportPointer), WaitForClient());   
    EXPECT_CALL((*transportPointer), InitializeProtocol()).Times(1);
    messageHandler->Initialize();

    // Message arrived from the client (stored in the internal queue)
    messageHandler->Notify(argumentsPointer);
    EXPECT_EQ(1llu, messageHandler->Pending());

    // Expect call to Runtime Read Register Method
    EXPECT_CALL((*transportPointer), SendMessage(::_)).WillOnce(testing::Invoke([&](shared_ptr<DebugMessage> argument)
    {
        uint16_t messageId = (*(argument->Buffer()))[0] | (*(argument->Buffer()))[1] << 0x08;
        // Error Code
        uint16_t targetError = (*(argument->Buffer()))[2] | (*(argument->Buffer()))[3] << 0x08;
      
        EXPECT_EQ(MessageID::MessageError, messageId);
        EXPECT_EQ(ErrorID::InvalidRegister, targetError);
    }));

    // Take message from the queue and process it
    messageHandler->ProcessMessages(runtimeMock, runnerMock);
    EXPECT_EQ(0llu, messageHandler->Pending());
}